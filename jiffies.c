/**
 * Module:   display jiffies in "file" in /proc
 * Purpose:  
 * Author:   Wade Hampton (based on examples) (DEPRECATED)
 * Date:     N/A
 * Notes:
 * 1)  This creates a "file" in /proc and displays jiffies.  It is 
 *     based on proc_fs.c which was based on procfs2.c.
 *     - WH changes:  
 *       -- no owner in proc_dir_entry
 *       -- used NULL for "parent" per TUXTHINK link, others fail
 * 2) Notes:  
 *    -- Ubuntu kernel headers in dirs like:  
 *         /usr/src/linux-headers-3.2.0-43/include/linux
 * 3) Links:
 *      http://tldp.org/LDP/lkmpg/2.6/html/x769.html#AEN806
 *      http://tuxthink.blogspot.com/2011/02/creating-readwrite-proc-entry.html
 *      http://www.ibm.com/developerworks/linux/library/l-proc/index.html
 *      http://www.gnugeneration.com/books/linux/2.6.20/procfs-guide/ch05.html
 * 4) This uses the deprecated /proc interface which was removed in
 *    3.x kernels.  Use the newer interface.  
 *     
 */

#include <linux/module.h>	/* Specifically, a module */
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/proc_fs.h>	/* Necessary because we use the proc fs */
#include <linux/jiffies.h>
#include <asm/uaccess.h>	/* for copy_from_user */

#define PROCFS_MAX_SIZE		1024
#define PROCFS_NAME 		"buffer1k"

/**
 * This structure hold information about the /proc file
 *
 */
static struct proc_dir_entry *Our_Proc_File;

/**
 * The buffer used to store character for this module
 *
 */
static char procfs_buffer[PROCFS_MAX_SIZE];

/**
 * The size of the buffer
 *
 */
static unsigned long procfs_buffer_size = 0;

/** jiffies at start */
static u64 j_start = 0;

/** 
 * This function is called then the /proc file is read
 *
 */
int 
procfile_read(char *buffer,
	      char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	int ret;
	
	printk(KERN_INFO "procfile_read (/proc/%s) called\n", PROCFS_NAME);
	
	if (offset > 0) {
		/* we have finished to read, return 0 */
		ret  = 0;
	} else {
		/* fill the buffer, return the buffer size */
		memcpy(buffer, procfs_buffer, procfs_buffer_size);
		ret = procfs_buffer_size;
	}

	return ret;
}

/**
 * This function is called with the /proc file is written
 *
 */
int procfile_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
	/* get buffer size */
	procfs_buffer_size = count;
	if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
		procfs_buffer_size = PROCFS_MAX_SIZE;
	}
	
	/* write data to the buffer */
	if ( copy_from_user(procfs_buffer, buffer, procfs_buffer_size) ) {
		return -EFAULT;
	}
	
	return procfs_buffer_size;
}

/**
 *This function is called when the module is loaded
 *
 */
int init_module()
{
	/* create the /proc file */
	Our_Proc_File = create_proc_entry(PROCFS_NAME, 0644, NULL);
	
	if (Our_Proc_File == NULL) {
	        remove_proc_entry(PROCFS_NAME, NULL);
	        // WH remove_proc_entry(PROCFS_NAME, &proc_root);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
			PROCFS_NAME);
		return -ENOMEM;
	}

	Our_Proc_File->read_proc  = procfile_read;
	Our_Proc_File->write_proc = procfile_write;
	// WH Our_Proc_File->owner 	  = THIS_MODULE;
	Our_Proc_File->mode 	  = S_IFREG | S_IRUGO;
	Our_Proc_File->uid 	  = 0;
	Our_Proc_File->gid 	  = 0;
	Our_Proc_File->size 	  = 37;

	/* read jiffies and put in buffer + log */
	j_start = get_jiffies_64();
	

	printk(KERN_INFO "/proc/%s created at J=%llu\n", 
	       PROCFS_NAME, (unsigned long long)j_start);	
	return 0;	/* everything is ok */
}

/**
 *This function is called when the module is unloaded
 *
 */
void cleanup_module()
{
    u64 j_end = get_jiffies_64();
    remove_proc_entry(PROCFS_NAME, NULL);
	
    // WH remove_proc_entry(PROCFS_NAME, &proc_root);
    printk(KERN_INFO "/proc/%s removed at J=%llu\n", 
	   PROCFS_NAME, (unsigned long long)j_end);
}
