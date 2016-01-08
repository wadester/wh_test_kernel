#!/usr/bin/make
# Module:   Makefile
# Purpose:  build module test programs based on LKM Howto
# Author:   WH
# Date:     3/5/2013
#
# Notes:
# 1)  Type make all to make these.  Note, w/o the GPL listed
#     as the module license, it "taints" the kernel.  Taint will
#     only be reset after rebooting.
#        cat /proc/sys/kernel/tainted
#
# 2)  Programs:
#       test1     very simple kernel module (taints kernel)
#       hello-2   demonstrate module_init(), module_exit() macros
#       hello-3   hello-2 plus license, author, description
#       proc_fs-old  /proc example, Linux 2.6 and earlier
#       proc_new     /proc example with jiffies, Linux 2.6.18 and later 
#                    (This works w/ CentOS 5 or later)
#       jiffies      /proc example with jiffies, deprecated /proc interface
#       kthread      sample kernel thread, uses old /proc (deprecated)
#       kthread2     expanded kernel thread, uses old /proc (deprecated)
#       kthread3     expanded kernel thread, new /proc interface
#       crash_evens  expanded seq_file example with proc data

obj-m += test1.o
obj-m += hello-2.o
obj-m += hello-3.o

# proc_fs-old, jiffies, kthread, kthread2 use old /proc API
#   which was deprecated and removed in 3.x kernels and later
#obj-m += proc_fs-old.o
# obj-m += jiffies.o
# obj-m += kthread.o
# obj-m += kthread2.o

obj-m += proc_new.o
obj-m += kthread3.o
obj-m += crash_evens.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

tar::
	tar cvzf ../kernel_tests.tgz *

