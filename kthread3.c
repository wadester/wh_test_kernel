/**
 * Module:   kthread3
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
 *
 * 4)  This uses the old /proc interface which is deprecated and removed
 *     in later kernels.  See newer example using different interface.    
 */

#include <linux/module.h>	/* Specifically, a module */
#include <linux/init.h>
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/fs.h>		/* for basic filesystem */
#include <linux/proc_fs.h>	/* Necessary because we use the proc fs */
#include <linux/seq_file.h>	/* for sequence files */
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>	/* for copy_from_user */
#include <linux/delay.h>

/** constants */
#define DRIVER_AUTHOR "Wade Hampton <w@example.com"
#define DRIVER_DESC   "Sample kthread test wh_kthread3"
#define PROCFS_NAME   "wh_kthread3"

/** module info */
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);	  /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);  /* What does this module do */


/** The proc file dir entry */
static struct proc_dir_entry *Our_Proc_File;

/** jiffies at start */
static u64 j_start = 0;

/** jiffie delta for thread 1 */
static u64 j_th1 = 0;

/** jiffie delta for thread 2 */
static u64 j_th2 = 0;

/** thread structure1 */
static struct task_struct *wh_kthread3_task;

/** thread structure1 */
static struct task_struct *wh_kthread3b_task;

/** Seemed to be no longer part of API, 
 *  got from http://lwn.net/Articles/98026/ 
 *  https://www.kernel.org/doc/Documentation/timers/timers-howto.txt
 *  new example:  this is in delay.h so use the kernel version
 */
/*
void msleep_interruptible(unsigned int msecs)
{
	unsigned long timeout = msecs_to_jiffies(msecs);

	while (timeout) {
		set_current_state(TASK_INTERRUPTIBLE);
		timeout = schedule_timeout(timeout);
	}
}
*/

/** display the current values and states */
static int 
kthread3_show(struct seq_file *m, void *v)
{
    wake_up_process(wh_kthread3_task);

    seq_printf(m, "Now:  %llu  T1=%llu  T2=%llu\n",
	       (unsigned long long) get_jiffies_64(),
	       (unsigned long long) j_th1, 
	       (unsigned long long) j_th2);
    return 0;
}

/** implement the open() call, pass the "show" function */
static int
kthread3_open(struct inode *inode, struct file *file)
{
    return single_open(file, kthread3_show, NULL);
}

/** file operations, open, read, seek, etc. */
static const struct file_operations kthread3_fops = {
    .owner	= THIS_MODULE,
    .open	= kthread3_open,
    .read	= seq_read,
    .llseek	= seq_lseek,
    .release	= single_release,
};

/** test slave thread */
int thread_wh_kthread3b(void *arg) {
    int should_stop = false;
    int cnt = 0;
    u64 j_now;
    while(1==1) {
	cnt++;
	should_stop = kthread_should_stop();
	if (should_stop) {
	    printk(KERN_INFO "thread2b %s stopping\n", PROCFS_NAME);
	    break;
	}
	j_now = get_jiffies_64(); 
	j_th2 = j_now - j_start;

	//schedule_timeout(HZ);
	//yield();
	msleep_interruptible(1000);
    }
    return(cnt);
}

/** test kernel thread */
int thread_wh_kthread3(void *arg) {
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
	j_th1 = j_now - j_start;

	//schedule_timeout(HZ);
	//yield();
	wake_up_process(wh_kthread3b_task);
	msleep_interruptible(1000);
    }
    return(cnt);
}

/** This function is called when the module is loaded */
int init_module()
{
    /* create the /proc file */
    Our_Proc_File = proc_create(PROCFS_NAME, 0, NULL, &kthread3_fops);     
    if (Our_Proc_File == NULL) {
	remove_proc_entry(PROCFS_NAME, NULL);
	// WH remove_proc_entry(PROCFS_NAME, &proc_root);
	printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
	       PROCFS_NAME);
	return -ENOMEM;
    }

    /** create the threads */
    wh_kthread3_task = kthread_create(thread_wh_kthread3,
				      NULL,  PROCFS_NAME);
    wh_kthread3b_task= kthread_create(thread_wh_kthread3b,
				      NULL,  PROCFS_NAME);
    wake_up_process(wh_kthread3_task);
    
    /* read jiffies and put in buffer + log */
    j_start = get_jiffies_64();
    
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
    // wake_up_process(wh_kthread3_task);
    res = kthread_stop(wh_kthread3_task);

    // WH remove_proc_entry(PROCFS_NAME, &proc_root);
    printk(KERN_INFO "/proc/%s removed at J=%llu DT=%llu r=%d\n", 
	   PROCFS_NAME, (unsigned long long)j_end, 
	   (unsigned long long)dt, res);

    res = kthread_stop(wh_kthread3b_task);

    // WH remove_proc_entry(PROCFS_NAME, &proc_root);
    printk(KERN_INFO "/proc/%s removed at J=%llu DT=%llu r=%d\n", 
	   PROCFS_NAME, (unsigned long long)j_end, 
	   (unsigned long long)dt, res);
}
