/* The program acceses the devices in the '/dev/' directory which 
 * support open, release (close), read and write operations
 * as FIFOs 
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

/* Data that will be passed through ioctl() (IOCTL_VALSET() and IOCTL_VALGET) */
struct ioctl_arg {
	unsigned int val;
};

/* Device file info */
#define DEVICE_NAME			"chardev_kfifo"
#define NUM_OF_DEVICE_FILES		2
#define BASE_MINOR			0

/* Program setup (half are write threads and the other read) */
#define W_THREAD_NUMBER_PER_GROUP	2
#define R_THREAD_NUMBER_PER_GROUP	2
#define SENTENCE_SIZE			32

/* Custom error codes */
#define SUCCESS				0
#define FAILURE				-1

/* File locations */
char device_file_name[NUM_OF_DEVICE_FILES][30] = {0};

/* Structure for sending data */
struct thread_info {
	int fd;
	int thread_num;
	char *device_file;
};

/* Thread that writes to FIFO */
void *w_thread_func(void *info) {
	
	/* Thread info */
	int fd = ((struct thread_info *)info)->fd;
	int thread_num = ((struct thread_info *)info)->thread_num;
	char *device_file = ((struct thread_info *)info)->device_file;
	int thread_group = thread_num / W_THREAD_NUMBER_PER_GROUP;
	char sentence[SENTENCE_SIZE] = {0};
	int ret = 0;
	
	// DELETE
	printf("Write thread %d GROUP: %d\n", thread_num, thread_group);
	
	/* Construct data and device file */
	snprintf(sentence, sizeof(sentence), "Data from write thread %d!", thread_num);
	
	/* Write to the device file */
	ret = write(fd, sentence, sizeof(sentence));
	if (ret < (int)sizeof(sentence)) printf("Write thread %d @%s: Failed to write string of size %d, written %d characters!\n", thread_num, device_file_name[thread_group], sizeof(sentence), ret); 
	else printf("Write thread %d @%s: Written %d characters!\n", thread_num, device_file_name[thread_group], sizeof(sentence));
	
}

/* Thread that reads from FIFO */
void *r_thread_func(void *info) {
	
	/* Thread info */
	int fd = ((struct thread_info *)info)->fd;
	int thread_num = ((struct thread_info *)info)->thread_num;
	char *device_file = ((struct thread_info *)info)->device_file;
	int thread_group = thread_num / R_THREAD_NUMBER_PER_GROUP;
	char sentence[SENTENCE_SIZE] = {0};
	int ret = 0;
	
	// DELETE
	printf("Read thread %d GROUP: %d\n", thread_num, thread_group);
	
	/* Write to the device file */
	ret = read(fd, sentence, sizeof(sentence));
	if (ret < (int)sizeof(sentence)) printf("Read thread %d @%s: Failed to read string of size %d, read %d characters!\n", thread_num, device_file_name[thread_group], sizeof(sentence), ret); 
	else printf("Read thread %d @%s: Read %d characters!\n", thread_num, device_file_name[thread_group], sizeof(sentence));
	
	/* Print the received string */
	printf("Read thread %d @%s: The read string from the FIFO was: '%s'\n", thread_num, device_file_name[thread_group], sentence);
	
}

int main() {
	
	/* File descriptors */
	int fd[NUM_OF_DEVICE_FILES];
	
	/* Thread identifiers */
	pthread_t w_thread[W_THREAD_NUMBER_PER_GROUP * NUM_OF_DEVICE_FILES];
	pthread_t r_thread[R_THREAD_NUMBER_PER_GROUP * NUM_OF_DEVICE_FILES];
	
	/* Structure pointers */
	struct thread_info *w_thread_info[W_THREAD_NUMBER_PER_GROUP * NUM_OF_DEVICE_FILES];
	struct thread_info *r_thread_info[R_THREAD_NUMBER_PER_GROUP * NUM_OF_DEVICE_FILES];
	
	printf("Beggining communication...\n");
	
	/* ============================================================================================================== */
	
	/* Open the device files as read-write */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		/* Device name */
		sprintf(device_file_name[minor], "/dev/" DEVICE_NAME "%u", minor);
		
		/* Open the device for reading and writing */
		fd[minor] = open(device_file_name[minor], O_RDWR);
		if (fd[minor] < 0) printf("Failed to open %s\n", device_file_name[minor]);
		else printf("Opened %s\n", device_file_name[minor]);
		
	}
	
	printf("Starting write threads...\n");
	
	/* ============================================================================================================== */
	
	/* Spawn write threads */
	for (int w_thread_num = 0; w_thread_num < W_THREAD_NUMBER_PER_GROUP * NUM_OF_DEVICE_FILES; w_thread_num++) {
		
		/* Get the group number which this thread belongs to */
		int minor = w_thread_num / W_THREAD_NUMBER_PER_GROUP;
		
		/* Allocate memory, fill thread info */
		w_thread_info[w_thread_num] = malloc(sizeof(struct thread_info));
		if (w_thread_info[w_thread_num] == NULL) break; 
		
		w_thread_info[w_thread_num]->fd = fd[minor];
		w_thread_info[w_thread_num]->thread_num = w_thread_num;
		w_thread_info[w_thread_num]->device_file = device_file_name[minor];
		
		/* Create thread */
		pthread_create(&w_thread[w_thread_num], NULL, w_thread_func, (void *)w_thread_info[w_thread_num]);
		
	}
	
	/* ============================================================================================================== */
	
	/* Wait for write treads */
	for (int w_thread_num = 0; w_thread_num < W_THREAD_NUMBER_PER_GROUP * NUM_OF_DEVICE_FILES; w_thread_num++) {
		
		if (w_thread_info[w_thread_num] == NULL) break;
		
		/* Join thread */
		pthread_join(w_thread[w_thread_num], NULL);
		
		/* Free memory */
		free(w_thread_info[w_thread_num]);
		
	}
	
	printf("Write threads finished! Starting read threads...\n");
	
	/* ============================================================================================================== */
	
	/* Spawn read threads */
	for (int r_thread_num = 0; r_thread_num < R_THREAD_NUMBER_PER_GROUP * NUM_OF_DEVICE_FILES; r_thread_num++) {
		
		/* Get the group number which this thread belongs to */
		int minor = r_thread_num / R_THREAD_NUMBER_PER_GROUP;
		
		/* Allocate memory, fill thread info */
		r_thread_info[r_thread_num] = malloc(sizeof(struct thread_info));
		if (r_thread_info[r_thread_num] == NULL) break; 
		
		r_thread_info[r_thread_num]->fd = fd[minor];
		r_thread_info[r_thread_num]->thread_num = r_thread_num;
		r_thread_info[r_thread_num]->device_file = device_file_name[minor];
		
		/* Create thread */
		pthread_create(&r_thread[r_thread_num], NULL, r_thread_func, (void *)r_thread_info[r_thread_num]);
		
	}
	
	/* ============================================================================================================== */
	
	/* Wait for read treads */
	for (int r_thread_num = 0; r_thread_num < R_THREAD_NUMBER_PER_GROUP * NUM_OF_DEVICE_FILES; r_thread_num++) {
		
		if (r_thread_info[r_thread_num] == NULL) break;
		
		/* Join thread */
		pthread_join(r_thread[r_thread_num], NULL);
		
		/* Free memory */
		free(r_thread_info[r_thread_num]);
		
	}
	
	printf("Read threads finished!\n");
	
	/* ============================================================================================================== */
	
	/* Close the device files */
	for (unsigned int minor = BASE_MINOR; minor < NUM_OF_DEVICE_FILES; minor++) {
		
		if (fd[minor] < 0) continue;
		
		/* Close the device */
		close(fd[minor]);
		printf("Closed %s\n", device_file_name[minor]);
			
	}
	
	printf("Communication ended!\n");
	
	return SUCCESS;
	
}
