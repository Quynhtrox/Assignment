// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>

static int sensor_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct device_node *np = dev->of_node;
    u32 threshold;

    if (!np) {
        dev_err(dev, "No device tree node found\n");
        return -EINVAL;
    }

    if (of_property_read_u32(np, "threshold", &threshold)) {
        dev_err(dev, "Failed to read threshold property\n");
        return -EINVAL;
    }

    dev_info(dev, "Probe successful! Threshold = %u\n", threshold);
    return 0;
}

static int sensor_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "Device removed\n");
    return 0;
}

static const struct of_device_id sensor_of_match[] = {
    { .compatible = "troc,troc-sensor", },
    {},
};
MODULE_DEVICE_TABLE(of, sensor_of_match);

static struct platform_driver my_sensor_driver = {
    .driver = {
        .name = "troc_sensor_driver",
        .of_match_table = sensor_of_match,
    },
    .probe = sensor_probe,
    .remove = sensor_remove,
};

module_platform_driver(my_sensor_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple platform driver for my-sensor");
