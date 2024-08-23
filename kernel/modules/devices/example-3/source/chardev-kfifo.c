/* The module creates character devices 'chardev-kfifo' in the '/dev/' directory which 
 * support open, release (close), read and write operations
 */

/* ====================================================================================================
 * INCLUDE DIRECTIVES
 * ====================================================================================================
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/kfifo.h>
#include <linux/version.h>

/* ====================================================================================================
 * PREPROCESSOR DEFINITIONS
 * ====================================================================================================
 */

/* Device file info */
#define DRIVER_NAME "chardev_kfifo_driver"
#define CLASS_NAME "chardev_kfifo_class"
#define DEVICE_NAME "chardev_kfifo"
#define NUM_OF_DEVICE_FILES 2
#define BASE_MINOR 0

/* Kernel FIFO information */

/* Kernel fifo is circular and uses modulo
 * operations to sequentialy write and read
 * data, therefore its size must be a power
 * of 2
 */

#define KFIFO_BUFFER_SIZE 512

/* Error codes */
#define SUCCESS 0
#define FAILURE -1

/* ====================================================================================================
 * GLOBAL VARIABLES 
 * ====================================================================================================
 */

/* Structure that holds information
 * about about the device file and
 * and kernel FIFO
 */

struct kernel_fifo_mutex_s {
	struct kfifo *kernel_fifo;
	struct mutex *kernel_mutex_r;
	struct mutex *kernel_mutex_w;
};

static unsigned int major = 0;								/* Variable for holding the major number */
static int alloc_ret = -1;								/* Holds return value from device registration */
static int cdev_ret = -1;								/* Holds return value from adding character device registration */
static struct class *device_class;							/* The class structure that describes the class of the device */
static struct cdev kfifo_cdev;								/* Variable for holding the character device file structure */	
static int allocated_kfifo_str_nr = 0;							/* Number of allocated kernel FIFO structures */
static int allocated_kfifo_str_buf_nr = 0;						/* Number of allocated kernel FIFO structure buffers */
static int allocated_mutex_r_str_nr = 0;						/* Number of allocated kernel read mutex structures */
static int allocated_mutex_w_str_nr = 0;						/* Number of allocated kernel write mutex structures */
static struct kernel_fifo_mutex_s kernel_fifo_mutex_array[NUM_OF_DEVICE_FILES] = {0};	/* Array of pointers to device structures */
static struct device *kfifo_device[NUM_OF_DEVICE_FILES] = {NULL};			/* Array of pointers to device structures */

/* ====================================================================================================
 * FILE OPERATIONS 
 * ====================================================================================================
 */

/* Read values from kfifo and write to user buffer */
static ssize_t device_kfifo_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
	
	/* Retreive the mutex and kfifo from
	 * from the file structure that was
	 * assigned to the process that opened
	 * the file
	 */
	struct kernel_fifo_mutex_s *kernel_fifo_mutex = filp->private_data;
	int retval = 0, copied = 0;
	
	/* Use mutex to prevent race condition */
	struct kfifo **kernel_fifo = &kernel_fifo_mutex->kernel_fifo;
	struct mutex **kernel_mutex_w = &kernel_fifo_mutex->kernel_mutex_w;
	
	/* Close the write lock, write data into FIFO, unlock the lock */
	if (mutex_lock_interruptible(*kernel_mutex_w)) return -ERESTARTSYS;
	if (kfifo_from_user(*kernel_fifo, (char __user *)buf, count, &copied)) retval = -EFAULT;
	else retval = copied;
	mutex_unlock(*kernel_mutex_w);
	
	return retval;
	
}

/* Read values from user buffer and write to kfifo */
static ssize_t device_kfifo_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
	
	/* Retreive the custom data structure
	 * saved in the file structure that was
	 * assigned to the process that opened
	 * the file
	 */
	struct kernel_fifo_mutex_s *kernel_fifo_mutex = filp->private_data;
	int retval = 0, copied = 0;
	
	/* Use mutex to prevent race condition */
	struct kfifo **kernel_fifo = &kernel_fifo_mutex->kernel_fifo;
	struct mutex **kernel_mutex_r = &kernel_fifo_mutex->kernel_mutex_r;
	
	/* Close the write lock, write data into FIFO, unlock the lock */
	if (mutex_lock_interruptible(*kernel_mutex_r)) return -ERESTARTSYS;
	if (kfifo_to_user(*kernel_fifo, (char __user *)buf, count, &copied)) retval = -EFAULT;
	else retval = copied;
	mutex_unlock(*kernel_mutex_r);
	
	return retval;
	
}

/* Destroy the data */
static int device_kfifo_close(struct inode *inode, struct file *filp) {
	
	/* Set the pointer to the kernel
	 * FIFO to NULL
	 */
	if (filp->private_data) {
		filp->private_data = NULL;
	}
	
	return 0; 
	
}

/* Create data structure by allocating memory on the kernel heap */
static int device_kfifo_open(struct inode *inode, struct file *filp) {
	
	/* Get the minor number so that
	 * we can associate the KFIFO
	 * with the right device file
	 */
	unsigned int minor = iminor(inode);
	
	/* Set private data to point
	 * to the structure that contains
	 * the kernel FIFO
	 */
	filp->private_data = &kernel_fifo_mutex_array[minor];
	
	return 0;
	
}

/* File operations structure, its members point to handler functions */
static struct file_operations fops = {
	
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0) 
	.owner = THIS_MODULE,
#endif 
	.open = device_kfifo_open,
	.release = device_kfifo_close,
	.read = device_kfifo_read,
	.write = device_kfifo_write,
	
};

/* ====================================================================================================
 * ENTRY AND EXIT FUNCTIONS - HELPER FUNCTIONS
 * ====================================================================================================
 */

/* Allocate mutexes */
static int allocate_mutexes(void) {
	
	/* Initialize kernel mutexes */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		struct mutex **kernel_mutex_r = &kernel_fifo_mutex_array[minor].kernel_mutex_r;
		
		*kernel_mutex_r = kmalloc(sizeof(struct mutex), GFP_KERNEL);
		if (*kernel_mutex_r == NULL) {
			pr_alert(DRIVER_NAME ": Error allocating memory for /dev/" DEVICE_NAME "%u device's mutex structure!\n", minor);
			return FAILURE;
		}		
		mutex_init(*kernel_mutex_r);
		
		allocated_mutex_r_str_nr++;
		pr_info(DRIVER_NAME ": Kernel read mutex for device /dev/" DEVICE_NAME "%u created.\n", minor);
	
	}
	
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		struct mutex **kernel_mutex_w = &kernel_fifo_mutex_array[minor].kernel_mutex_w;
		
		*kernel_mutex_w = kmalloc(sizeof(struct mutex), GFP_KERNEL);
		if (*kernel_mutex_w == NULL) {
			pr_alert(DRIVER_NAME ": Error allocating memory for /dev/" DEVICE_NAME "%u device's mutex structure!\n", minor);
			return FAILURE;
		}		
		mutex_init(*kernel_mutex_w);		
		
		allocated_mutex_w_str_nr++;
		pr_info(DRIVER_NAME ": Kernel write mutex for device /dev/" DEVICE_NAME "%u created.\n", minor);
		
	}
	
	return SUCCESS;
	
}


/* Allocate kernel FIFOs*/
static int allocate_fifos(void) {
	
	/* Initialize kernel FIFOs */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		struct kfifo **kernel_fifo = &kernel_fifo_mutex_array[minor].kernel_fifo;
		
		*kernel_fifo = kmalloc(sizeof(struct kfifo), GFP_KERNEL);
		if (*kernel_fifo == NULL) {
			pr_alert(DRIVER_NAME ": Error allocating memory for /dev/" DEVICE_NAME "%u device's kernel FIFO structure!\n", minor);
			return FAILURE;
		}
		allocated_kfifo_str_nr++;
		
	}
	
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		struct kfifo **kernel_fifo = &kernel_fifo_mutex_array[minor].kernel_fifo;
		
		if (kfifo_alloc(*kernel_fifo, KFIFO_BUFFER_SIZE, GFP_KERNEL)) {
			pr_alert(DRIVER_NAME ": Error allocating memory for /dev/" DEVICE_NAME "%u device's kernel FIFO structure buffer!\n", minor);
			return FAILURE;
		}
		allocated_kfifo_str_buf_nr++;
		
		pr_info(DRIVER_NAME ": Kernel FIFO for device /dev/" DEVICE_NAME "%u created.\n", minor);
		
	}
	
	return SUCCESS;
	
}

/* Create devices */
static int create_devices(void) {
	
	/* Creates device files in the /dev/ directory where programs can interact with the device */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		/* Device name */
		char kfifo_device_file_name[30] = {0};
		snprintf(kfifo_device_file_name, sizeof(kfifo_device_file_name), DEVICE_NAME "%u", minor);
		
		/* Create device */
		kfifo_device[minor] = device_create(device_class, NULL, MKDEV(major, minor), NULL, kfifo_device_file_name);
		if (IS_ERR(kfifo_device[minor])) {
			pr_alert(DRIVER_NAME ": Error in device file creation!\n");
			return FAILURE;
		}
		
		pr_info(DRIVER_NAME ": Device file /dev/%s created.\n", kfifo_device_file_name);
		
	}
	
	return SUCCESS;
	
}

/* Create class */
static int create_class(void) {
	
	/* Create a device class */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
	device_class = class_create(CLASS_NAME);
#else
	device_class = class_create(THIS_MODULE, CLASS_NAME);
#endif
	if (IS_ERR(device_class)) {
		pr_alert(DRIVER_NAME ": Error in device class creation!\n");
		return FAILURE;
	}
	pr_info(DRIVER_NAME ": Character device class created named '%s'.\n", CLASS_NAME);
	
	return SUCCESS;
	
}

/* Add character device from kernel */
static int add_character_device_to_kernel(dev_t* dev) {
	
	/* Initialize a cdev_t structure with 
	 * the given file operations and add the
	 * structure to the system
	 */
	cdev_init(&kfifo_cdev, &fops);
	cdev_ret = cdev_add(&kfifo_cdev, *dev, NUM_OF_DEVICE_FILES);
	
	/* If adding failure return error code */
	if (cdev_ret) {
		pr_alert(DRIVER_NAME ": Could not add the character device to kernel!\n");
		return FAILURE;
	}
	pr_info(DRIVER_NAME ": Character device added to kernel.\n");
	
	return SUCCESS;
	
}

/* Register character device in kernel */
static int register_character_device(dev_t* dev) {

	/* Allocate a range of character device
	 * numbers, the major number will be selected dynamically
	 */
	alloc_ret = alloc_chrdev_region(dev, BASE_MINOR, NUM_OF_DEVICE_FILES, DRIVER_NAME);
	
	/* If allocation failure return error code */
	if (alloc_ret) {
		pr_alert(DRIVER_NAME ": Could not register character device!\n\n");
		return FAILURE;
	}
	pr_info(DRIVER_NAME ": Registered character device.\n");
	
	/* Read the major number that was allocated
	 * from the dev_t structure 
	 */
	major = MAJOR(*dev);
	
	return SUCCESS;
	
}

/* Initialization function */
static int init(void) {
	
	dev_t dev;
	
	if (register_character_device(&dev) == FAILURE) return FAILURE;
	if (add_character_device_to_kernel(&dev) == FAILURE) return FAILURE;
	if (create_class() == FAILURE) return FAILURE;
	if (create_devices() == FAILURE) return FAILURE;
	if (allocate_fifos() == FAILURE) return FAILURE;
	if (allocate_mutexes() == FAILURE) return FAILURE;
	
	return SUCCESS;
	
}

/* Unregister character device from kernel */
static void unregister_character_device(void) {
	
	if (alloc_ret == 0) unregister_chrdev_region(MKDEV(major, BASE_MINOR), NUM_OF_DEVICE_FILES);
	pr_info(DRIVER_NAME ": Unregistered character device.\n");
	
}

/* Remove character device from kernel */
static void remove_character_device_from_kernel(void) {
	
	if (cdev_ret == 0) cdev_del(&kfifo_cdev);
	pr_info(DRIVER_NAME ": Character device removed from kernel.\n");
	
}

/* Destroy class */
static void destroy_class(void) {
	
	if (!IS_ERR(device_class)) class_destroy(device_class);
	pr_info(DRIVER_NAME ": Character device class destroyed.\n");
	
}

/* Destroy devices */
static void destroy_devices(void) {
	
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) if (!IS_ERR(kfifo_device[minor])) device_destroy(device_class, MKDEV(major, minor));
	pr_info(DRIVER_NAME ": All device files destroyed.\n");
	
}

/* Free kernel FIFOs*/
static void free_fifos(void) {
	
	for (unsigned int minor = BASE_MINOR; minor < allocated_kfifo_str_buf_nr; minor++) {
		struct kfifo **kernel_fifo = &kernel_fifo_mutex_array[minor].kernel_fifo;
		kfifo_free(*kernel_fifo);
	}
	
	for (unsigned int minor = BASE_MINOR; minor < allocated_kfifo_str_nr; minor++) {
		struct kfifo **kernel_fifo = &kernel_fifo_mutex_array[minor].kernel_fifo;
		kfree(*kernel_fifo);
	}
	
	pr_info(DRIVER_NAME ": All kernel FIFOs destroyed.\n");
	
}

/* Free mutexes */
static void free_mutexes(void) {
	
	for (unsigned int minor = BASE_MINOR; minor < allocated_mutex_w_str_nr; minor++) {
		struct mutex **kernel_mutex_w = &kernel_fifo_mutex_array[minor].kernel_mutex_w;
		kfree(*kernel_mutex_w);
	}
	
	for (unsigned int minor = BASE_MINOR; minor < allocated_mutex_r_str_nr; minor++) {
		struct mutex **kernel_mutex_r = &kernel_fifo_mutex_array[minor].kernel_mutex_r;
		kfree(*kernel_mutex_r);
	}
	
	pr_info(DRIVER_NAME ": All kernel mutexes destroyed.\n");
	
}

/* Cleanup function */
static void cleanup(void) {

	free_mutexes();	
	free_fifos();
	destroy_devices();
	destroy_class();
	remove_character_device_from_kernel();
	unregister_character_device();
	
}

/* ====================================================================================================
 * ENTRY AND EXIT FUNCTIONS 
 * ====================================================================================================
 */

/* Entry function */
static int __init device_kfifo_init(void) {
	
	pr_alert(DRIVER_NAME ": Initializing...\n");
	if (init() == FAILURE) goto error;
	pr_alert(DRIVER_NAME ": Initialization successful! Major number is: %d.\n", major);
	
	return 0;
	
error:
	
	pr_alert(DRIVER_NAME ": Initialization unsuccessful! Cleanup initiated...\n");
	cleanup();
	pr_alert(DRIVER_NAME ": Cleanup finished.\n");
	
	return -1;
	
}

/* Exit function */
static void __exit device_kfifo_exit(void) {
	
	pr_alert(DRIVER_NAME ": Cleanup initiated...\n");
	cleanup();
	pr_alert(DRIVER_NAME ": Cleanup finished.\n");
	
}

/* Register entry function, exit function and license*/
module_init(device_kfifo_init);
module_exit(device_kfifo_exit);

/* ====================================================================================================
 * MODULE INFO 
 * ====================================================================================================
 */

/* Module information */
MODULE_AUTHOR("Iwan Culumovic");
MODULE_DESCRIPTION("Creates character device files with associated kernel FIFOs for reading and writing");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
