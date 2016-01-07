/**
 * Module:   kthread2
 * Purpose:  create two kernel threads
 * Author:   Wade Hampton (based on examples)
 * Date:     N/A
 * Notes:
 * 1)  Create two kernel threads.
 * 2)  Ref: 
 *      http://lwn.net/Articles/65178/
 *      https://www.kernel.org/doc/htmldocs/device-drivers/API-wake-up-process.html
 *
 * 3)  Notes
 *     - kthread_stop() sets the flag which should be tested with 
 *       kthread_do_stop(), then the thread should return 0 or a negative
 *       number -- it should not call do-exit().
 */

#include <linux/module.h>	/* Specifically, a module */
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/proc_fs.h>	/* Necessary because we use the proc fs */
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>	/* for copy_from_user */

/** constants */
#define DRIVER_AUTHOR "Wade Hampton <w@example.com"
#define DRIVER_DESC   "Sample kthread test wh_kthread2"
#define PROCFS_MAX_SIZE		1024
#define PROCFS_NAME 	        "wh_kthread2"

/** module info */
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);	/* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);	/* What does this module do */


/** The buffer used to store character for this module */
static char procfs_buffer[PROCFS_MAX_SIZE];

/** The size of the buffer */
static unsigned long procfs_buffer_size = 0;

/** The proc file dir entry */
static struct proc_dir_entry *Our_Proc_File;

/** jiffies at start */
static u64 j_start = 0;

/** thread structure1 */
static struct task_struct *wh_kthread2_task;

/** thread structure1 */
static struct task_struct *wh_kthread2b_task;

/** seems this is no longer part of API, 
 *  got from http://lwn.net/Articles/98026/ 
 */
void msleep_interruptible(unsigned int msecs)
{
	unsigned long timeout = msecs_to_jiffies(msecs);

	while (timeout) {
		set_current_state(TASK_INTERRUPTIBLE);
		timeout = schedule_timeout(timeout);
	}
}

/** test slave thread */
int thread_wh_kthread2b(void *arg) {
    int should_stop = false;
    int cnt = 0;
    //u64 j_now;
    while(1==1) {
	cnt++;
	should_stop = kthread_should_stop();
	if (should_stop) {
	    printk(KERN_INFO "thread2b %s stopping\n", PROCFS_NAME);
	    break;
	}
	/* j_now = get_jiffies_64(); */
	/* snprintf(procfs_buffer, sizeof(procfs_buffer), */
	/* 	 "S=%llu N=%llu", j_start, j_now);  */
	/* procfs_buffer_size=strlen(procfs_buffer); */

	//schedule_timeout(HZ);
	//yield();
	msleep_interruptible(1000);
    }
    return(cnt);
}

/** test kernel thread */
int thread_wh_kthread2(void *arg) {
    int should_stop = false;
    int cnt = 0;
    u64 j_now;
    while(1==1) {
	cnt++;
	should_stop = kthread_should_stop();
	if (should_stop) {
	    printk(KERN_INFO "thread2 %s stopping\n", PROCFS_NAME);
	    break;
	}
	j_now = get_jiffies_64();
	snprintf(procfs_buffer, sizeof(procfs_buffer),
		 "S=%llu N=%llu", j_start, j_now); 
	procfs_buffer_size=strlen(procfs_buffer);

	//schedule_timeout(HZ);
	//yield();
	wake_up_process(wh_kthread2b_task);
	msleep_interruptible(1000);
    }
    return(cnt);
}

/** read from the proc file -- copy buffer to user space */
int 
procfile_read(char *buffer,
	      char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	int ret;
	if (offset > 0) {
		/* we have finished to read, return 0 */
		ret  = 0;
	} else {
		/* fill the buffer, return the buffer size */
		memcpy(buffer, procfs_buffer, procfs_buffer_size);
		ret = procfs_buffer_size;
	}
	wake_up_process(wh_kthread2_task);

	return ret;
}

/** This function is called when the module is loaded */
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
	// Our_Proc_File->write_proc = procfile_write;
	// WH Our_Proc_File->owner 	  = THIS_MODULE;
	Our_Proc_File->mode 	  = S_IFREG | S_IRUGO;
	Our_Proc_File->uid 	  = 0;
	Our_Proc_File->gid 	  = 0;
	Our_Proc_File->size 	  = 37;

	/** create the threads */
	wh_kthread2_task = kthread_create(thread_wh_kthread2,
					  NULL,  PROCFS_NAME);
	wh_kthread2b_task= kthread_create(thread_wh_kthread2b,
					  NULL,  PROCFS_NAME);
	wake_up_process(wh_kthread2_task);

	/* read jiffies and put in buffer + log */
	j_start = get_jiffies_64();

	snprintf(procfs_buffer, sizeof(procfs_buffer), 
		 "start=%llu", (unsigned long long)j_start);
	procfs_buffer_size=strlen(procfs_buffer);

	printk(KERN_INFO "/proc/%s created at J=%llu (HZ=%ld)\n", 
	       PROCFS_NAME, (unsigned long long)j_start,
	       (unsigned long)HZ);	
	return 0;	/* everything is ok */
}

/** This function is called when the module is unloaded */
void cleanup_module()
{
    int res = 0;
    u64 j_end = get_jiffies_64();
    unsigned long long dt=j_end=j_start;

    /* clean up proc */
    remove_proc_entry(PROCFS_NAME, NULL);
    
    /* stop thread */
    // wake_up_process(wh_kthread2_task);
    res = kthread_stop(wh_kthread2_task);

    // WH remove_proc_entry(PROCFS_NAME, &proc_root);
    printk(KERN_INFO "/proc/%s removed at J=%llu DT=%llu r=%d\n", 
	   PROCFS_NAME, (unsigned long long)j_end, 
	   (unsigned long long)dt, res);

    res = kthread_stop(wh_kthread2b_task);

    // WH remove_proc_entry(PROCFS_NAME, &proc_root);
    printk(KERN_INFO "/proc/%s removed at J=%llu DT=%llu r=%d\n", 
	   PROCFS_NAME, (unsigned long long)j_end, 
	   (unsigned long long)dt, res);
}
