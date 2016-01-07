/**
 * Module:   hello-3
 * Purpose:  demonstrate the license feature
 * Author:   Wade Hampton (based on examples)
 * Date:     N/A
 * Notes:
 * 1)  This is a very simple Linux kernel module with a simple printk
 * 2)  Ref:  See README.md
 *     
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */

#define DRIVER_AUTHOR "Wade Hampton <w@example.com"
#define DRIVER_DESC   "Sample driver hello-3"

/* 
 * Get rid of taint message by declaring code as GPL. 
 */
MODULE_LICENSE("GPL");

/*
 * Or with defines, like this:
 */
MODULE_AUTHOR(DRIVER_AUTHOR);	  /* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);  /* What does this module do */

/*  
 *  This module uses /dev/testdevice.  The MODULE_SUPPORTED_DEVICE macro might
 *  be used in the future to help automatic configuration of modules, but is 
 *  currently unused other than for documentation purposes.
 */
MODULE_SUPPORTED_DEVICE("testdevice");

static int __init hello_3_init(void)
{
	printk(KERN_INFO "Hello, world 3\n");
	return 0;
}

static void __exit hello_3_exit(void)
{
	printk(KERN_INFO "Goodbye, world 3\n");
}

module_init(hello_3_init);
module_exit(hello_3_exit);
