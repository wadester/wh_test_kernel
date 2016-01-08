# wh_test_kernel
Linux Kernel module tests

Summary:
========

These are examples of progamming Linux kernel modules.  The
original versions were based on the The Linux Kernel Module 
Programming Guide from the Linux Doc Project (TLDP).

History:
========

N/A (created 5/20/2013)

Notes:
======

1)  Links:
*   TLDP: http://www.tldp.org/LDP/lkmpg/2.6/html/
*   Module: https://en.wikipedia.org/wiki/Loadable_kernel_module

2)  Without listing the module as GPL, it will be "tainted"
    which is set and can only be reset on reboot:
       cat /proc/sys/kernel/tainted

3)  The 


These will be expanded over time....


Files:
======

  test1        very simple kernel module (taints kernel)
  hello-2      demonstrate module_init(), module_exit() macros
  hello-3      hello-2 plus license, author, description
  proc_fs-old  /proc example, Linux 2.6 and earlier
  proc_new     /proc example with jiffies using new /proc interface
               Linux 2.6.18 and later (CentOS 5 suppoorted)
  jiffies      /proc example with jiffies, deprecated /proc interface
  kthread      sample kernel thread, uses old /proc (deprecated)
  kthread2     expanded kernel thread, uses old /proc (deprecated)
  kthread3     expanded kernel thread, new /proc interface

Notes:
======

*  CentOS 5-7:  src is in /usr/src/kernels/`uname -r`

*  Ubuntu 14:  src is in /usr/src/linux-headers-`uname -r`

