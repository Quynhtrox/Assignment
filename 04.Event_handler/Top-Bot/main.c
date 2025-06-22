#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>

static int method = 1; // 1 = tasklet, 2 = workqueue
module_param(method, int, 0444);
MODULE_PARM_DESC(method, "Bottom half method: 1=Tasklet, 2=Workqueue");

#define MY_IRQ 1

/* Workqueue */ 
static void bh_work_func(struct work_struct *work);
static DECLARE_WORK(bh_work, bh_work_func);

/* Tasklet */
static void bh_tasklet_func(unsigned long data);
static struct tasklet_struct bh_tasklet;

static irqreturn_t my_irq_handler(int irq, void *dev_id);

/* Bottom Half: Workqueue */
static void bh_work_func(struct work_struct *work) {
    pr_info("[workqueue] Bottom half running @ jiffies=%lu\n", jiffies);
}

/* Bottom Half: Tasklet */
static void bh_tasklet_func(unsigned long data) {
    pr_info("[tasklet] Bottom half running @ jiffies=%lu\n", jiffies);
}

/* Top Half */
static irqreturn_t my_irq_handler(int irq, void *dev_id) {
    pr_info("TOP HALF: IRQ %d received @ jiffies=%lu\n", irq, jiffies);

    if (method == 1)
        tasklet_schedule(&bh_tasklet);
    else if (method == 2)
        schedule_work(&bh_work);
    else
        pr_warn("Unknown method: %d\n", method);

    return IRQ_HANDLED;
}

static int __init mod_init(void) {
    pr_info("Module loaded (method=%d)\n", method);

    if (method == 1)
        tasklet_init(&bh_tasklet, bh_tasklet_func, 0);

    if (request_irq(MY_IRQ, my_irq_handler, IRQF_SHARED, "event_handler_demo", (void *)&my_irq_handler)) {
        pr_err("Failed to register IRQ %d\n", MY_IRQ);
        return -EIO;
    }

    pr_info("Registered IRQ %d. Press keys to trigger.\n", MY_IRQ);
    return 0;
}

static void __exit mod_exit(void) {
    free_irq(MY_IRQ, (void *)&my_irq_handler);

    if (method == 1)
        tasklet_kill(&bh_tasklet);
    else if (method == 2)
        flush_work(&bh_work);

    pr_info("Module unloaded.\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Troc");
MODULE_DESCRIPTION("Demo top half - bottom half");