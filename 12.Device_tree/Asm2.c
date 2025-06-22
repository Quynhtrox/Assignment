// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

static int my_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    struct device_node *child;
    int age = 0;
    const char *dev_name = NULL;
    const __be32 *arr;
    int len;

    if (of_property_read_u32(np, "age", &age) == 0)
        dev_info(&pdev->dev, "age = %d\n", age);

    if (of_property_read_string(np, "dev_name", &dev_name) == 0)
        dev_info(&pdev->dev, "dev_name = %s\n", dev_name);

    arr = of_get_property(np, "troc-array", &len);
    if (arr && len > 0) {
        int i, count = len / sizeof(u32);
        dev_info(&pdev->dev, "troc-array = ");
        for (i = 0; i < count; i++)
            pr_cont("0x%x ", be32_to_cpu(arr[i]));
        pr_cont("\n");
    }

    of_platform_populate(np, NULL, NULL, &pdev->dev);

    // Read child node
    for_each_child_of_node(np, child) {
        dev_info(&pdev->dev, "Found child: %s\n", child->name);

        u32 child_age = 0;
        const char *child_dev_name = NULL;
        const __be32 *child_reg;

        if (of_property_read_u32(child, "age", &child_age) == 0)
            dev_info(&pdev->dev, "child: age = %d\n", child_age);

        if (of_property_read_string(child, "dev_name", &child_dev_name) == 0)
            dev_info(&pdev->dev, "child: dev_name = %s\n", child_dev_name);

        child_reg = of_get_property(child, "reg", NULL);
        if (child_reg)
            dev_info(&pdev->dev, "child: reg = 0x%x 0x%x\n",
                     be32_to_cpu(child_reg[0]), be32_to_cpu(child_reg[1]));
    }

    return 0;
}

static int my_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "Device removed\n");
    return 0;
}

static const struct of_device_id my_dt_ids[] = {
    { .compatible = "troc,Q-device", },
    {}
};
MODULE_DEVICE_TABLE(of, my_dt_ids);

static struct platform_driver my_driver = {
    .driver = {
        .name = "my_driver",
        .of_match_table = my_dt_ids,
    },
    .probe = my_probe,
    .remove = my_remove,
};

module_platform_driver(my_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Troc");
MODULE_DESCRIPTION("Assignment 2 - Device Tree platform driver");
MODULE_VERSION("1.0");