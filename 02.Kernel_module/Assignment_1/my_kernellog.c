#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init my_kernellog_init(void)
{
    printk(KERN_INFO "1. This is a Kernel info\n");
    printk(KERN_WARNING "2. This is a Kernel warning\n");
    printk(KERN_ERR "3. This is a Kern error\n");
    printk(KERN_DEBUG "4. This is a Kern debug\n");

    return 0;
}

static void __exit my_kernellog_exit(void)
{
    pr_info("Good bye\n");
}

module_init(my_kernellog_init);
module_exit(my_kernellog_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("QuynhTrox");
MODULE_DESCRIPTION("kernel log");