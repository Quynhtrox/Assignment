#include <linux/module.h>
#include <linux/io.h>
#include "gpio.h"

uint32_t __iomem *gpio0_base_addr;

static int __init gpio_init(void) {
    gpio0_base_addr = ioremap(GPIO0_ADDR_BASE, GPIO0_ADDR_SIZE);
    if (!gpio0_base_addr) {
        pr_err("Failed to ioremap GPIO0\n");
        return -ENOMEM;
    }

    *(gpio0_base_addr + GPIO_OE_OFFSET / 4) &= ~GPIO0_30;
    *(gpio0_base_addr + GPIO_SETDATAOUT_OFFSET / 4) |= GPIO0_30;

    pr_info("Hello kernel module\n");
    return 0;
}

static void __exit gpio_exit(void) {
    *(gpio0_base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= GPIO0_30;
    iounmap(gpio0_base_addr);

    pr_info("Exit kernel module\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Troc");
MODULE_DESCRIPTION("Load and unload kernel module to control led P9_11");
MODULE_VERSION("FIRST");
