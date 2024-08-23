/* The program acceses the devices in the '/dev/' directory which 
 * support open, release (close) and ioctl operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

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
#define DEVICE_NAME "chardev_ioctl"
#define NUM_OF_DEVICE_FILES 2
#define BASE_MINOR 0

/* Custom error codes */
#define SUCCESS 0
#define FAILURE -1

int main() {
	
	/* File descriptors */
	int fd[NUM_OF_DEVICE_FILES];
	
	/* Data that will be sent and received with ioctl to the device files */
	struct ioctl_arg data_write[NUM_OF_DEVICE_FILES] = {0}, data_read[NUM_OF_DEVICE_FILES] = {0};
	
	printf("==================================================\n");
	
	/* ============================================================================================================== */
	
	/* Open the device files as read-write */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		/* Device name */
		char ioctl_device_file_name[30] = {0};
		sprintf(ioctl_device_file_name, "/dev/" DEVICE_NAME "%u", minor);
		
		/* Open the device for reading and writing */
		fd[minor] = open(ioctl_device_file_name, O_RDWR);
		if (fd[minor] < 0) printf("Failed to open /dev/" DEVICE_NAME "%u\n", minor);
		else printf("Opened /dev/" DEVICE_NAME "%u\n", minor);
		
	}
	
	printf("==================================================\n");
	
	/* ============================================================================================================== */
	
	/* Test writing struct arg data to devices using ioctl() function */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		if (fd[minor] < 0) continue;
	
		/* Set data to be sent to be: (minor + 1) * 100 */
		data_write[minor].val = (minor + 1) * 100;
		
		/* Write data to device */
		if (ioctl(fd[minor], IOCTL_VALSET, &data_write[minor])) printf("[/dev/" DEVICE_NAME "%u]: ioctl() subcommand IOCTL_VALSET error!\n", minor);
		else printf("[/dev/" DEVICE_NAME "%u]: ioctl() subcommand IOCTL_VALSET executed! Data written from structure. The written number was %u\n", minor, data_write[minor].val);
		
	}
	
	printf("==================================================\n");
	
	/* ============================================================================================================== */
	
	/* Test reading struct arg data from devices using ioctl() function */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		if (fd[minor] < 0) continue;
		
		/* Read data from device */
		if (ioctl(fd[minor], IOCTL_VALGET, &data_read[minor])) printf("[/dev/" DEVICE_NAME "%u]: ioctl() subcommand IOCTL_VALGET error!\n", minor);
		else printf("[/dev/" DEVICE_NAME "%u]: ioctl() subcommand IOCTL_VALGET executed! Data read into structure. The read number was %u\n", minor, data_read[minor].val);
		
	}
	
	printf("==================================================\n");
	
	/* ============================================================================================================== */
	
	/* Close the device files */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		if (fd[minor] < 0) continue;
		
		/* Close the device */
		close(fd[minor]);
		printf("Closed /dev/" DEVICE_NAME "%u\n", minor);
			
	}
	
	printf("==================================================\n");
	
	return SUCCESS;
	
}
