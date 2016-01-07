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
#       test1:    very basic Hello World
#       hello-2:  demonstrate module_init(), module_exit() macros
#


obj-m += test1.o
obj-m += hello-2.o
obj-m += hello-3.o
# obj-m += proc_fs.o
# obj-m += jiffies.o
# obj-m += kthread.o
# obj-m += kthread2.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

tar::
	tar cvzf ../kernel_tests.tgz *

