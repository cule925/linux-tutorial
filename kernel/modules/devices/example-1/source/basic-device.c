/* The module creates a character device 'chardev' in the '/dev/' directory which 
 * supports open, release (close), read and write operations
 * 
 * Command 'sudo cat /dev/basic_device' will return "Hello World"
 * Command 'echo 1 | sudo tee /dev/basic_device' will put an alert in the kernel log
 * buffer ('sudo dmesg')
 */

#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <asm/errno.h>

/* Declarations */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

/* Definitions */
#define SUCCESS 0
#define CLASS_NAME "basic_device_class"		/* Class name */
#define DEVICE_NAME "basic_device"		/* Device name */
#define BUF_LEN 80				/* Message size */

/* Major number assigned to the driver */
static int major;

enum {
	CDEV_NOT_USED = 0,
	CDEV_EXCLUSIVE_OPEN = 1,
};

/* This variable is prone to race conditions if atomic operations are not used 
 * because multiple programs could access the same device at the same time
 */
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

/* The class structure that describes the class of the device */
static struct class *cls;

/* File operations structure, its members point to handler functions */
static struct file_operations chardev_fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
};

/* Entry function */
static int __init chardev_init(void) {
	
	/* Register a device in the kernel, dynamically allocate a major number */
	major = register_chrdev(0, DEVICE_NAME, &chardev_fops);
	
	/* If allocation failed exit the module */
	if (major < 0) {
		pr_alert("Registering char device failed with %d\n", major);
		return major;
	}
	
	pr_info("I was assigned major number %d.\n", major);
	
	/* Create a class for the device */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
	cls = class_create(CLASS_NAME);
#else
	cls = class_create(THIS_MODULE, CLASS_NAME);
#endif
	
	/* Creates a device file in the /dev/ directory where programs can interact with the device */
	device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
	pr_info("Device created on /dev/%s\n", DEVICE_NAME);
	return SUCCESS; 
	
}

/* Exit function */
static void __exit chardev_exit(void) {
	
	/* Delete the device file and class, also, unregister the device */
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, DEVICE_NAME);
	
}

/* Device open operation implementation */
static int device_open(struct inode *inode, struct file *file) {
	
	/* Check if global flag is 0 (closed) and if it is set it to 1 (open), returns the old value
	 * that was stored, if the device is already opened return (this ensures that only one program
	 * can access the device until it is closed)
	 */
	if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) return -EBUSY;
	
	return SUCCESS;
	
}

/* Device release (close) operation implementation */
static int device_release(struct inode *inode, struct file *file) {
	
	/* Closing the device means atomically setting the global flag to 0 (closed) */
	atomic_set(&already_open, CDEV_NOT_USED);
	
	return SUCCESS;
	
}

/* Device read operation implementation */
static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
	
	/* The message */
	static char msg[] = "Hello World!\n";
	
	/* Counter and pointer */
	int bytes_read = 0;
	const char *msg_ptr = msg;
	
	/* If trying to read from end of message ('\0'), return */
	if (!*(msg_ptr + *offset)) {
		
		*offset = 0;
		return 0;
		
	}
	
	/* Set the pointer to the right place */
	msg_ptr += *offset;
	
	/* Begin writing to user space till length is 0 or pointer is '\0' */
	while (length && *msg_ptr) {
		
		/* Copy data from kernel space to user space */
		put_user(*(msg_ptr++), buffer++);
		length--;
		bytes_read++;
		
	}
	
	/* Set the new offset */
	*offset += bytes_read;
	
	return bytes_read;
	
}

/* Device write operation implementation */
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off) {
	
	pr_alert("Sorry, this operation is not supported.\n");
	return -EINVAL;
	
}

/* Register entry function and exit function */
module_init(chardev_init);
module_exit(chardev_exit);

/* Module information */
MODULE_AUTHOR("Iwan Culumovic");
MODULE_DESCRIPTION("Creates one character device file that when read outputs");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
