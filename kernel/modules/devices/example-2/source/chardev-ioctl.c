/* The module creates character devices 'chardev-ioctl' in the '/dev/' directory which 
 * support open, release (close) and ioctl operations
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

/* The define preprocessor directives need to be included in the user program
 * so that the user program knows how to access the device file using ioctl
 * with the magic number and command indexes
 */

/* Data that will be passed through ioctl() (IOCTL_VALSET() and IOCTL_VALGET) */
struct ioctl_arg {
	unsigned int val;
};

/* To prevent errors from issuing
 * right commands to a wrong device
 * a unique command set number is
 * given
 */
#define IOC_MAGIC '\x66' /* Equal to character 'f' */

/* The macros take the magic number or type of command,
 * the number of the command and finally the structure
 */

/* Macro _IOW() defines a command with a designated
 * index (0 in this case) of the custom '\x66'
 * command set that transfers data from user memory
 * space to kernel memory space
 */

/* Macro _IOR() defines a command with a designated
 * index (1 in this case) of the custom '\x66'
 * command set that transfers data from kernel memory
 * space to user memory space */

#define IOCTL_VALSET _IOW(IOC_MAGIC, 0, struct ioctl_arg) 		/* A struct ioctl_arg will be passed from user to kernel with this command */
#define IOCTL_VALGET _IOR(IOC_MAGIC, 1, struct ioctl_arg) 		/* A struct ioctl_arg will be passed from kernel to user with this command */

/* Device file info */
#define DRIVER_NAME "chardev_ioctl_driver"
#define CLASS_NAME "chardev_ioctl_class"
#define DEVICE_NAME "chardev_ioctl"
#define NUM_OF_DEVICE_FILES 2
#define BASE_MINOR 0

static unsigned int ioctl_major = 0;					/* Variable for holding the major number */
static struct class *device_class;					/* The class structure that describes the class of the device */
static struct cdev ioctl_cdev;						/* Variable for holding the character device file structure */
static struct device *ioctl_device[NUM_OF_DEVICE_FILES] = {NULL};	/* Array of pointers to device structures */

/* Custom structure that contains one
 * one character an a Read-Write lock,
 * this structure will be allocated for
 * each file pointer that points to an
 * open file
 */
struct ioctl_data_with_lock {
	unsigned int val;
	rwlock_t lock;
};

/* Executing code depending on the command received */
static long ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
	
	/* Retreive the custom data structure
	 * saved in the file structure that was
	 * assigned to the process that opened
	 * the file
	 */
	struct ioctl_data_with_lock *private_data = filp->private_data;
	
	int retval = 0;
	unsigned int val;
	
	/* Fill block of memory with 0 */
	struct ioctl_arg data;
	memset(&data, 0, sizeof(data));
	
	/* Execute code depending on command */
	switch (cmd) {
		case IOCTL_VALSET:
			
			/* Copy struct arg argument from
			 * user space into kernel space
			 */
			if (copy_from_user(&data, (int __user *)arg, sizeof(data))) {
				retval = -EFAULT;
				goto done;
			}
			pr_info("IOCTL set value: %u\n", data.val);
			
			/* Close the Read-Write
			 * lock in the structure
			 * with a Write lock which
			 * means that no other process
			 * can also close it with a Read
			 * or Write lock, after that 
			 * write the value and then open
			 * the lock
			 */
			write_lock(&private_data->lock);
			private_data->val = data.val;
			write_unlock(&private_data->lock);
			
			break;
			
		case IOCTL_VALGET:
			
			/* Close the Read-Write
			 * lock in the structure
			 * with a Read lock which
			 * means that other processes
			 * can also close it with a Read
			 * lock but not a Write lock, 
			 * read the value and then open
			 * the lock
			 */
			read_lock(&private_data->lock);
			val = private_data->val;
			read_unlock(&private_data->lock);
			
			/* Copy struct arg argument from
			 * kernel space into user space
			 */
			data.val = val;
			pr_info("IOCTL get value: %u\n", data.val);
			if (copy_to_user((int __user *)arg, &data, sizeof(data))) {
				retval = -EFAULT;
				goto done;
			}
			
			break;
			
		default:
			
			retval = -ENOTTY;
			
	}
	
done:
	return retval;
	
}

/* Destroy the data */
static int device_close(struct inode *inode, struct file *filp) {
	
	pr_info("%s call.\n", __func__);
	
	/* Free the allocated memory using the
	 * file structure that was assigned
	 * when the process opened the file
	 * and containes the pointer to the
	 * allocated memory
	 */
	if (filp->private_data) {
		kfree(filp->private_data);
		filp->private_data = NULL;
		
	}
	
	return 0; 
	
}

/* Create data structure by allocating memory on the kernel heap */
static int device_open(struct inode *inode, struct file *filp) {
	
	pr_info("%s call.\n", __func__);
		
	/* Allocate memory for data structure */
	struct ioctl_data_with_lock *private_data;
	private_data = kmalloc(sizeof(struct ioctl_data_with_lock), GFP_KERNEL);
	
	/* If allocation unsuccesful, return */
	if (private_data == NULL) return -ENOMEM;
	
	/* Initialize the Read-Write lock */
    	rwlock_init(&private_data->lock);
	
	/* Set data in the custom structure and 
	 * save the pointer to the structure
	 * in the file structure that gets assigned
	 * to the process that opened the file
	 */
	private_data->val = 0xFF;
	filp->private_data = private_data;
	
	return 0;
	
}

/* File operations structure, its members point to handler functions */
static struct file_operations fops = {
	
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0) 
	.owner = THIS_MODULE,
#endif 
	.open = device_open,
	.release = device_close,
	.unlocked_ioctl = ioctl,
	
};

/* Entry function */
static int __init ioctl_init(void) {
	
	dev_t dev;
	int alloc_ret = -1;
	int cdev_ret = -1;
	
	/* Allocate a range of character device
	 * numbers, the major number will be selected dynamically
	 */
	alloc_ret = alloc_chrdev_region(&dev, BASE_MINOR, NUM_OF_DEVICE_FILES, DRIVER_NAME);
	
	/* If allocation failure return */
	if (alloc_ret) goto error;
	
	/* Read the major number that was allocated
	 * from the dev_t structure 
	 */
	ioctl_major = MAJOR(dev);
	
	/* Initialize a cdev_t structure with 
	 * the given file operations and add the
	 * structure to the system
	 */
	cdev_init(&ioctl_cdev, &fops);
	cdev_ret = cdev_add(&ioctl_cdev, dev, NUM_OF_DEVICE_FILES);
	
	/* If adding failure return */
	if (cdev_ret) goto error;
	pr_alert("%s driver(major: %d) installed.\n", DRIVER_NAME, ioctl_major);
	
	/* Create a device class */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
	device_class = class_create(CLASS_NAME);
#else
	device_class = class_create(THIS_MODULE, CLASS_NAME);
#endif
	if (IS_ERR(device_class)) goto error;
	
	/* Creates device files in the /dev/ directory where programs can interact with the device */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		/* Device name */
		char ioctl_device_file_name[30] = {0};
		snprintf(ioctl_device_file_name, sizeof(ioctl_device_file_name), DEVICE_NAME "%u", minor);
		
		/* Create device */
		ioctl_device[minor] = device_create(device_class, NULL, MKDEV(ioctl_major, minor), NULL, ioctl_device_file_name);
		if(IS_ERR(ioctl_device[minor])) goto error;
		pr_info("Device file /dev/%s created!\n", ioctl_device_file_name);
		
	}
	
	return 0;
	
	/* If there was an error delete the created
	 * device files, delete the class of devices
	 * remove the cdev_t structure from the system
	 * and deallocate the allocated numbers
	 */
error:
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++)
		if (!IS_ERR(ioctl_device[minor])) device_destroy(device_class, MKDEV(ioctl_major, minor));
	if (!IS_ERR(device_class)) class_destroy(device_class);
	if (cdev_ret == 0) cdev_del(&ioctl_cdev);
	if (alloc_ret == 0) unregister_chrdev_region(MKDEV(ioctl_major, BASE_MINOR), NUM_OF_DEVICE_FILES);
	
	return -1;
	
}

/* Exit function */
static void __exit ioctl_exit(void) {
	
	/* Delete the created device files, delete
	 * the class of devices remove the cdev_t
	 * structure from the system and deallocate
	 * the allocated numbers
	 */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) device_destroy(device_class, MKDEV(ioctl_major, minor));
	class_destroy(device_class);
	cdev_del(&ioctl_cdev);
	unregister_chrdev_region(MKDEV(ioctl_major, BASE_MINOR), NUM_OF_DEVICE_FILES);
	
	pr_alert("%s driver removed.\n", DRIVER_NAME);
	
}

/* Register entry function and exit function */
module_init(ioctl_init);
module_exit(ioctl_exit);

/* Module information */
MODULE_AUTHOR("Iwan Culumovic");
MODULE_DESCRIPTION("Creates character device files with associated structures that can be edited via ioctl");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
