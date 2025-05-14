#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

void function_a(void);

static int __init moduleB_init(void)
{
    pr_info("Hello World from Module B !\n");
    function_a();
    return 0;
}

static void __exit moduleB_exit(void)
{
    pr_info("Good bye World from Module B\n");
}

module_init(moduleB_init);
module_exit(moduleB_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("QuynhTrox");
MODULE_DESCRIPTION("Module B");