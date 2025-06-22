#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/kthread.h>

#define GPIO0_ADDR_BASE     0x44E07000
#define GPIO0_ADDR_END      0x44E07FFF
#define GPIO0_ADDR_SIZE     (GPIO0_ADDR_END - GPIO0_ADDR_BASE + 1)

#define GPIO1_ADDR_BASE     0x4804C000
#define GPIO1_ADDR_END      0x4804CFFF
#define GPIO1_ADDR_SIZE     (GPIO1_ADDR_END - GPIO1_ADDR_BASE + 1)

#define GPIO_OE_OFFSET              0x134
#define GPIO_CLEARDATAOUT_OFFSET    0x190
#define GPIO_SETDATAOUT_OFFSET      0x194
#define GPIO_DATAIN_OFFSET          0x138

#define GPIO0_30    (1 << 30)   // LED - P9_11
#define GPIO1_28    (1 << 28)   // Button - P9_12

static void __iomem *gpio0_base;
static void __iomem *gpio1_base;
static struct task_struct *poll_thread;

static int poll_fn(void *data)
{
    bool last_state = 0;

    while (!kthread_should_stop()) {
        u32 val = readl(gpio1_base + GPIO_DATAIN_OFFSET);

        bool pressed = !(val & GPIO1_28);  // LOW = pressed

        if (pressed != last_state) {
            if (pressed) {
                pr_info("Button pressed → LED ON\n");
                writel(GPIO0_30, gpio0_base + GPIO_SETDATAOUT_OFFSET);
            } else {
                pr_info("Button released → LED OFF\n");
                writel(GPIO0_30, gpio0_base + GPIO_CLEARDATAOUT_OFFSET);
            }
            last_state = pressed;
        }

        msleep(50);  // polling delay
    }
    return 0;
}

static int __init gpio_init(void)
{
    gpio0_base = ioremap(GPIO0_ADDR_BASE, GPIO0_ADDR_SIZE);
    gpio1_base = ioremap(GPIO1_ADDR_BASE, GPIO1_ADDR_SIZE);

    if (!gpio0_base || !gpio1_base) {
        pr_err("Failed to ioremap GPIOs\n");
        return -ENOMEM;
    }

    // LED output (clear bit in OE)
    writel(readl(gpio0_base + GPIO_OE_OFFSET) & ~GPIO0_30, gpio0_base + GPIO_OE_OFFSET);

    // Button input (set bit in OE)
    writel(readl(gpio1_base + GPIO_OE_OFFSET) | GPIO1_28, gpio1_base + GPIO_OE_OFFSET);

    // Start polling thread
    poll_thread = kthread_run(poll_fn, NULL, "gpio_button_poll");
    if (IS_ERR(poll_thread)) {
        pr_err("Failed to create polling thread\n");
        return PTR_ERR(poll_thread);
    }

    pr_info("GPIO driver loaded: P9_11=LED, P9_12=Button\n");
    return 0;
}

static void __exit gpio_exit(void)
{
    if (poll_thread)
        kthread_stop(poll_thread);

    // Turn off LED
    writel(GPIO0_30, gpio0_base + GPIO_CLEARDATAOUT_OFFSET);

    iounmap(gpio0_base);
    iounmap(gpio1_base);

    pr_info("GPIO driver unloaded\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Troc");
MODULE_DESCRIPTION("LED on/off with button using GPIO direct access (ioremap)");
