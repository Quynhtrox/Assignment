#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

void function_a(void)
{
    pr_info("This funtion is from module A\n");
}

EXPORT_SYMBOL(function_a);

static int __init moduleA_init(void)
{
    pr_info("Hello World from A !\n");
    return 0;
}

static void __exit moduleA_exit(void)
{
    pr_info("Good bye from A\n");
}

module_init(moduleA_init);
module_exit(moduleA_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Quynhtrox");
MODULE_DESCRIPTION("Module_A");
