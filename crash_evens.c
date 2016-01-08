/**
 * Module:   crash_evens
 * Purpose:  to demonstrate more elaborate use of seq_file
 * Author:   ?
 * Date:     ?
 * Notes:
 * 1)  Based on:
 *       http://www.crashcourse.ca/sites/default/files/crash_evens.c
 * 2)  WH changes:
 *       - documentation added including header and these notes
 *       - replaced create_proc_entry() with proc_create()
 *       - additional printk statements on module load/unload
 *       - macro to define module name
 * 3)  Typical run (cat /var/log/messages or /var/log/syslog (ubuntu))
 *            [snip] Entering start(), pos = 0.
 *            [snip] In start(), even_ptr = e7f00668.
 *            [snip] In show(), even = 0.
 *            [snip] In next(), v = e7f00668, pos = 0.
 *            [snip] In show(), even = 2.
 *            [snip] In next(), v = e7f00668, pos = 1.
 *            [snip] In show(), even = 4.
 *            [snip] In next(), v = e7f00668, pos = 2.
 *            [snip] In show(), even = 6.
 *            [snip] In next(), v = e7f00668, pos = 3.
 *            [snip] In show(), even = 8.
 *            [snip] In next(), v = e7f00668, pos = 4.
 *            [snip] Entering stop().
 *            [snip] v is null.
 *            [snip] In stop(), even_ptr = e7f00668.
 *            [snip] Freeing and clearing even_ptr.
 *            [snip] Entering start(), pos = 5.
 *            [snip] Apparently, we're done.
 *            [snip] Entering stop().
 *            [snip] v is null.
 *            [snip] In stop(), even_ptr =   (null).
 *            [snip] even_ptr is already null.
 *
 *     Note that even when next() returns NULL and stop() is called,
 *     start() is called again to make sure there is nothing else to 
 *     output.  This boundary condition MUST be handled gracefully
 *     for example the start() check for >= limit and the stop()
 *     check for the null pointer.  Code must be setup to support
 *     being called multiple times!  See the linked article.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>      // for kmalloc() dynamic allocation

/** define name of proc file system */
#define PROCFS_NAME  "evens"

/** module parameter "limit=n", default=10, number of even values to display */
static int limit = 10;
module_param(limit, int, S_IRUGO);

/** even value, kmalloc'ed data */
static int* even_ptr;

/** start method for the sequence file */
static void *
ct_seq_start(struct seq_file *s, loff_t *pos)
{
    printk(KERN_INFO "Entering start(), pos = %Ld.\n", *pos);

    if ((*pos) >= limit) {     // are we done?
        printk(KERN_INFO "Apparently, we're done.\n");
        return NULL;
    }

    //  Allocate an integer to hold our increasing even value.

    even_ptr = kmalloc(sizeof(int), GFP_KERNEL);

    if (!even_ptr)     // fatal kernel allocation error
        return NULL;

    printk(KERN_INFO "In start(), even_ptr = %pX.\n", even_ptr);
    *even_ptr = (*pos) * 2;
    return even_ptr;
} 

/** actually display the value -- this is the output function 
 *  limit is 1 page of data, typ 4096 bytes
 */
static int
ct_seq_show(struct seq_file *s, void *v)
{
    printk(KERN_INFO "In show(), even = %d.\n", *((int*)v));
    seq_printf(s, "The current value of the even number is %d\n",
        *((int*)v));
    return 0;
}

/** next function for the sequence */
static void *
ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    int* val_ptr;

    printk(KERN_INFO "In next(), v = %pX, pos = %Ld.\n", v, *pos);

    (*pos)++;                // increase my position counter
    if ((*pos) >= limit)     // are we done?
         return NULL;

    val_ptr = (int *) v;     // address of current even value
    (*val_ptr) += 2;         // increase it by two

    return v;
} 

/** stop function for the sequence, cleanup */


static void
ct_seq_stop(struct seq_file *s, void *v)
{
    printk(KERN_INFO "Entering stop().\n");

    if (v) {
        printk(KERN_INFO "v is %pX.\n", v);
    } else {
        printk(KERN_INFO "v is null.\n");
    }

    printk(KERN_INFO "In stop(), even_ptr = %pX.\n", even_ptr);

    if (even_ptr) {
        printk(KERN_INFO "Freeing and clearing even_ptr.\n");
        kfree(even_ptr);
        even_ptr = NULL;
    } else {
        printk(KERN_INFO "even_ptr is already null.\n");
    }
} 

/** sequence operations
 *  note these were not required for the simple examples 
 */
static struct seq_operations ct_seq_ops = {
    .start = ct_seq_start,
    .next  = ct_seq_next,
    .stop  = ct_seq_stop,
    .show  = ct_seq_show
};

/** open file creating the sequence with options */
static int
ct_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &ct_seq_ops);
};

static struct file_operations ct_file_ops = {
    .owner   = THIS_MODULE,
    .open    = ct_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = seq_release
};

/** module init, create the proc entry */
static int
ct_init(void)
{
    struct proc_dir_entry *entry;

    /* entry = create_proc_entry("evens", 0, NULL); */
    entry = proc_create(PROCFS_NAME, 0, NULL, &ct_file_ops);

    /* if (entry) */
    /*     entry->proc_fops = &ct_file_ops; */
    if (entry == NULL) {
	printk(KERN_INFO "crash_evens:  failed to load module\n");
	return -ENOMEM;
    }

    printk(KERN_INFO "crash_evens:  module created\n");
    return 0;
}

/** module cleanup */
static void
ct_exit(void)
{
    remove_proc_entry(PROCFS_NAME, NULL);
    printk(KERN_INFO "crash_evens:  module removed\n");
}

/** module macros for init/exit and license */
module_init(ct_init);
module_exit(ct_exit);

MODULE_LICENSE("GPL"); 

