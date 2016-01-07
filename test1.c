/**
 * Module:   test1
 * Purpose:  the simplest kernel module
 * Author:   Wade Hampton (based on examples)
 * Date:     N/A
 * Notes:
 * 1)  This is a very simple Linux kernel module with a simple printk
 * 2)  Ref:  See README.md
 *     
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

int init_module(void)
{
	printk(KERN_INFO "Hello world 1.\n");

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}
