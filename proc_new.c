/**
 * Module:   proc_new
 * Purpose:  Kernel 3.x new PROC API (from examples)
 * Author:   Internet
 * Date:     N/A
 * Notes:
 * 1)  This uses the new Linux 3.x proc API and a "sequence file"
 * 2)  Ref:  
 *       http://lwn.net/Kernel/LDD3/   Linux Device Drivers e-book
 *       https://www.linux.com/learn/linux-training/37985-the-kernel-newbie-corner-kernel-debugging-using-proc-qsequenceq-files-part-1
 *       http://kernelnewbies.org/Documents/SeqFileHowTo
 * 
 *       From the kernelnewbies article:  "The "seq_file" interface to the /proc filesystem 
 *       was introduced in Linux 2.4.15-pre3 and Linux 2.4.13-ac8...".  It is 
 *       in CentOS 5 and later, recent Fedora, and recent Ubuntu.  
 *
 *       See:  https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/commit/?id=80e928f7ebb958f4d79d4099d1c5c0a015a23b93
 *
 *       The 3.x Linux kernel removed the old create_proc_entry(PROCFS_NAME, 0644, NULL);
 *       function and method in deference to using proc_create(...) and 
 *       typically "seq_file".  Note that my very old code used the
 *       old method (circa 2000).
 *      
 */

/************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/fs.h>		// for basic filesystem
#include <linux/proc_fs.h>	// for the proc filesystem
#include <linux/seq_file.h>	// for sequence files
#include <linux/jiffies.h>	// for jiffies

/* proc directory entry for this module */
static struct proc_dir_entry* jif_file;


/** display the current jiffies settings */
static int 
jif_show(struct seq_file *m, void *v)
{
    seq_printf(m, "%llu\n",
	       (unsigned long long) get_jiffies_64());
    return 0;
}

/** implement the open() call, pass the "show" function */
static int
jif_open(struct inode *inode, struct file *file)
{
    return single_open(file, jif_show, NULL);
}

/** file operations, open, read, seek, etc. */
static const struct file_operations jif_fops = {
    .owner	= THIS_MODULE,
    .open	= jif_open,
    .read	= seq_read,
    .llseek	= seq_lseek,
    .release	= single_release,
};

/** initialization function, called on module load, 
 *  creates the proc entry and returns 0 on OK
 */
static int __init 
jif_init(void)
{
    jif_file = proc_create("jif", 0, NULL, &jif_fops);
    
    if (!jif_file) {
	printk(KERN_INFO "PROC_NEW:  ERROR creating /proc/jif\n");
	return -ENOMEM;
    }
    printk(KERN_INFO "PROC_NEW:  created /proc/jif\n");    
    return 0;
}

/** cleanup/exit function, called on module removal */
static void __exit
jif_exit(void)
{
    printk(KERN_INFO "PROC_NEW:  Exiting, removing /proc/jif\n");
    remove_proc_entry("jif", NULL);
}

/** standard macros for the init/exit function and license.  
 *  define license as GPL to prevent a "tainted" kernel
 */
module_init(jif_init);
module_exit(jif_exit);

MODULE_LICENSE("GPL"); 
