#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device/class.h>
#include <linux/device.h>

#define MAX_DEVICE 5
#define MAX_DEVICE_BUFFER_SIZE 256

#include "Q_ioctl.h"

typedef enum device_permission {
    RDONLY,
    WRONLY,
    RDWR
} device_permission_e;

typedef struct pdev_infor {
    const char *name;
    int id;
    int size;
    device_permission_e perm;
} pdev_infor_t;

typedef struct troc_device {
    struct cdev cdev_obj;
    char buffer[MAX_DEVICE_BUFFER_SIZE];
    int actual_size;
    pdev_infor_t pdev_obj;
} troc_device_t;

typedef struct troc_driver {
    dev_t dev_num;
    struct class *troc_class;
    int cdev_num;
    struct platform_driver pdrv;
} troc_driver_t;

static troc_driver_t my_driver;

static int troc_open(struct inode *inode_ptr, struct file *file_ptr) {
    troc_device_t *dev = container_of(inode_ptr->i_cdev, troc_device_t, cdev_obj);
    file_ptr->private_data = dev;
    pr_info("TROC Device opened (minor = %d)\n", iminor(inode_ptr));
    return 0;
}

static int troc_release(struct inode *inode_ptr, struct file *file_ptr) {
    pr_info("TROC Device closed (minor = %d)\n", iminor(inode_ptr));
    return 0;
}

static ssize_t troc_read(struct file *file_ptr, char __user *user_buffer, size_t size, loff_t *offset) {
    troc_device_t *dev = file_ptr->private_data;
    size_t bytes_to_read;

    if (*offset >= dev->actual_size) {
        pr_info("TROC Read offset beyond data. Returning 0.\n");
        return 0;
    }

    bytes_to_read = min(size, (size_t)(dev->actual_size - *offset));

    if (copy_to_user(user_buffer, dev->buffer + *offset, bytes_to_read))
        return -EFAULT;

    *offset += bytes_to_read;
    pr_info("TROC Read %zu bytes from device\n", bytes_to_read);
    return bytes_to_read;
}

static ssize_t troc_write(struct file *file_ptr, const char __user *user_buffer, size_t size, loff_t *offset) {
    troc_device_t *dev = file_ptr->private_data;
    size_t bytes_to_write;

    if (*offset >= MAX_DEVICE_BUFFER_SIZE)
        return -ENOMEM;

    bytes_to_write = min(size, (size_t)(MAX_DEVICE_BUFFER_SIZE - *offset));

    if (copy_from_user(dev->buffer + *offset, user_buffer, bytes_to_write))
        return -EFAULT;

    *offset += bytes_to_write;
    if (*offset > dev->actual_size)
        dev->actual_size = *offset;

    pr_info("TROC Wrote %zu bytes to device\n", bytes_to_write);
    return bytes_to_write;
}

static long troc_ioctl(struct file *file_ptr, unsigned int cmd, unsigned long arg) {
    troc_device_t *dev = file_ptr->private_data;

    switch (cmd) {
        case Q_CLEAR_BUFFER:
            memset(dev->buffer, 0, MAX_DEVICE_BUFFER_SIZE);
            dev->actual_size = 0;
            pr_info("TROCIOCTL Cleared buffer\n");
            return 0;
        case Q_GET_BUFFER_SIZE:
            if (copy_to_user((int __user *)arg, &dev->actual_size, sizeof(int)))
                return -EFAULT;
            pr_info("TROCIOCTL Reported buffer size = %d\n", dev->actual_size);
            return 0;
        default:
            pr_info("TROCIOCTL Invalid command received\n");
            return -EINVAL;
    }
}

static struct file_operations troc_fops = {
    .owner = THIS_MODULE,
    .open = troc_open,
    .release = troc_release,
    .read = troc_read,
    .write = troc_write,
    .unlocked_ioctl = troc_ioctl,
};

static int pdrv_function_probe(struct platform_device *pdev) {
    pdev_infor_t *info = pdev->dev.platform_data;
    dev_t dev_num = my_driver.dev_num + info->id;

    pr_info("TROC PROBE Detected device %s (id=%d, size=%d)\n",
            info->name, info->id, info->size);

    troc_device_t *dev = devm_kzalloc(&pdev->dev, sizeof(troc_device_t), GFP_KERNEL);
    if (!dev) return -ENOMEM;

    dev->pdev_obj = *info;

    cdev_init(&dev->cdev_obj, &troc_fops);
    cdev_add(&dev->cdev_obj, dev_num, 1);

    device_create(my_driver.troc_class, NULL, dev_num, NULL, info->name);

    pdev->dev.driver_data = dev;
    my_driver.cdev_num++;

    pr_info("TROC PROBE Device created: /dev/%s (minor=%d)\n", info->name, info->id);
    return 0;
}

static int pdrv_function_remove(struct platform_device *pdev) {
    troc_device_t *dev = pdev->dev.driver_data;
    dev_t dev_num = dev->cdev_obj.dev;

    device_destroy(my_driver.troc_class, dev_num);
    cdev_del(&dev->cdev_obj);
    my_driver.cdev_num--;

    pr_info("TROC REMOVE Removed device: %s (minor=%d)\n",
            dev->pdev_obj.name, dev->pdev_obj.id);
    return 0;
}

static int __init troc_platform_driver_init(void) {
    pr_info("TROC Loading platform driver module\n");

    alloc_chrdev_region(&my_driver.dev_num, 0, MAX_DEVICE, "troc_region");
    my_driver.troc_class = class_create("troc_class");

    my_driver.pdrv.driver.name = "trocdevice";
    my_driver.pdrv.driver.owner = THIS_MODULE;
    my_driver.pdrv.probe = pdrv_function_probe;
    my_driver.pdrv.remove = pdrv_function_remove;

    platform_driver_register(&my_driver.pdrv);

    pr_info("TROC Driver registered with major %d\n", MAJOR(my_driver.dev_num));
    return 0;
}

static void __exit troc_platform_driver_exit(void) {
    pr_info("TROC Unloading platform driver module\n");

    platform_driver_unregister(&my_driver.pdrv);
    class_destroy(my_driver.troc_class);
    unregister_chrdev_region(my_driver.dev_num, MAX_DEVICE);

    pr_info("TROC Driver unregistered successfully\n");
}

module_init(troc_platform_driver_init);
module_exit(troc_platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Troc");
MODULE_DESCRIPTION("IOCTL-enabled Platform Driver with logging");
