#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include "Q_ioctl.h"

#define DEVICE_NAME     "baquynhdev"
#define MAX_BUFFER_SIZE 256

typedef struct baquynh_dev_data {
    char buffer[MAX_BUFFER_SIZE];
    int actual_size;
    struct cdev my_cdev;
} baquynh_dev_data;

typedef struct character_driver {
    dev_t dev_num;
    baquynh_dev_data baquynh_devs;
    struct class *my_class;
} cdr_t;

static cdr_t my_driver;

static int func_open(struct inode *inode, struct file *file) {
    struct baquynh_dev_data *my_dev_data = container_of(inode->i_cdev, struct baquynh_dev_data, my_cdev);
    file->private_data = my_dev_data;
    pr_info("baquynh: Device /dev/%s opened\n", DEVICE_NAME);
    return 0;
}

static int func_release(struct inode *inode, struct file *file) {
    pr_info("baquynh: Device /dev/%s closed\n", DEVICE_NAME);
    return 0;
}

static ssize_t func_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    struct baquynh_dev_data *my_dev_data = file->private_data;

    if (*ppos >= MAX_BUFFER_SIZE)
        return 0;

    size_t bytes_to_write = min((size_t)(MAX_BUFFER_SIZE - *ppos), count);

    if (bytes_to_write == 0)
        return 0;

    if (copy_from_user(my_dev_data->buffer + *ppos, buf, bytes_to_write))
        return -EFAULT;

    *ppos += bytes_to_write;
    if (*ppos > my_dev_data->actual_size)
        my_dev_data->actual_size = *ppos;

    return bytes_to_write;
}

static ssize_t func_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    struct baquynh_dev_data *my_dev_data = file->private_data;

    if (*ppos >= my_dev_data->actual_size)
        return 0;

    size_t bytes_to_read = min((size_t)(my_dev_data->actual_size - *ppos), count);

    if (copy_to_user(buf, my_dev_data->buffer + *ppos, bytes_to_read))
        return -EFAULT;

    *ppos += bytes_to_read;
    return bytes_to_read;
}

static long func_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct baquynh_dev_data *my_dev_data = file->private_data;

    switch (cmd) {
    case Q_CLEAR_BUFFER:
        my_dev_data->actual_size = 0;
        memset(my_dev_data->buffer, 0, MAX_BUFFER_SIZE);
        pr_info("baquynh: IOCTL - Buffer cleared\n");
        break;
    
    case Q_GET_BUFFER_SIZE:
        if (copy_to_user((int __user *)arg, &my_dev_data->actual_size, sizeof(int)))
            return -EFAULT;
        pr_info("baquynh: IOCTL - Get buffer size = %d\n", my_dev_data->actual_size);
        break;

    default:
        pr_info("baquynh: Unknown IOCTL command\n");
        return -EINVAL;
    }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = func_open,
    .release = func_release,
    .write = func_write,
    .read = func_read,
    .unlocked_ioctl = func_ioctl,
};

static int __init my_cdev_init(void) {
    pr_info("baquynh: Module loading...\n");

    alloc_chrdev_region(&my_driver.dev_num, 0, 1, DEVICE_NAME);
    my_driver.my_class = class_create("troc");

    cdev_init(&my_driver.baquynh_devs.my_cdev, &fops);
    cdev_add(&my_driver.baquynh_devs.my_cdev, my_driver.dev_num, 1);

    device_create(my_driver.my_class, NULL, my_driver.dev_num, NULL, "%s", DEVICE_NAME);

    pr_info("baquynh: Created device /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit my_cdev_exit(void) {
    device_destroy(my_driver.my_class, my_driver.dev_num);
    class_destroy(my_driver.my_class);
    cdev_del(&my_driver.baquynh_devs.my_cdev);
    unregister_chrdev_region(my_driver.dev_num, 1);
    pr_info("baquynh: Module unloaded\n");
}

module_init(my_cdev_init);
module_exit(my_cdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Troc");
MODULE_DESCRIPTION("Character device with IOCTL");
