/**
 * Module:   hello-2
 * Purpose:  use module_init() and module_exit() macros
 * Author:   Wade Hampton (based on examples)
 * Date:     N/A
 * Notes:
 * 1)   This is preferred over using init_module() and cleanup_module().
 * 2)  Ref:  See README.md
 *     
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */

static int __init hello_2_init(void)
{
	printk(KERN_INFO "Hello, world 2\n");
	return 0;
}

static void __exit hello_2_exit(void)
{
	printk(KERN_INFO "Goodbye, world 2\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);
