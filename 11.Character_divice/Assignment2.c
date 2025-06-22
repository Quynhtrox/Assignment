#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME     "baquynhdev"
#define MAX_BUFFER_SIZE 256

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;

static char kernel_buffer[MAX_BUFFER_SIZE];
static int actual_buffer_size = 0;

static char *my_devnode(const struct device *dev, umode_t *mode) {
    if (mode)
        *mode = 0666;
    return NULL;
}

static int func_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "baquynh: Device opened\n");
    return 0;
}

static int func_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "baquynh: Device closed\n");
    return 0;
}

static ssize_t func_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    printk(KERN_INFO "baquynh: Device request write\n");

    int bytes_to_write;

    if (*ppos >= MAX_BUFFER_SIZE) {
        printk(KERN_WARNING "baquynh: Write offset %lld exceeds buffer size %d\n", *ppos, MAX_BUFFER_SIZE);
        return 0;
    }

    bytes_to_write = min((size_t)(MAX_BUFFER_SIZE - *ppos), count);
    if (bytes_to_write == 0) {
        printk(KERN_INFO "baquynh: No byte to write at current offset.\n");
        return 0;
    }

    copy_from_user(kernel_buffer + *ppos, buf, bytes_to_write);

    *ppos += bytes_to_write;
    if (*ppos > actual_buffer_size) {
        actual_buffer_size = * ppos;
    }

    printk(KERN_INFO "baquynh: Wrote %d bytes to device. Current size: %d\n",bytes_to_write, actual_buffer_size);
    return bytes_to_write;
}

static ssize_t func_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    printk(KERN_INFO "baquynh: Device request read (count = %zu)\n", count);

    int bytes_to_read;
    if (*ppos >= actual_buffer_size) {
        printk(KERN_INFO "baquynh: Read offset %lld beyond current data size %d. End of file.\n", *ppos, actual_buffer_size);
        return 0;
    }

    bytes_to_read = min((size_t)(actual_buffer_size - *ppos), count);
    copy_to_user(buf, kernel_buffer + *ppos, bytes_to_read);
    *ppos += bytes_to_read;

    printk(KERN_INFO "baquynh: Read %d bytes from device.\n", bytes_to_read);
    return bytes_to_read;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = func_open,
    .release = func_release,
    .write = func_write,
    .read = func_read,
};

static int __init my_cdev_init(void) {
    printk(KERN_INFO "baquynh: Module loading ...\n");
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

    my_class = class_create("troc");

    my_class->devnode = my_devnode;

    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev_num, 1);

    device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    printk(KERN_INFO "baquynh: Created device\n");
    
    return 0;
}

static void __exit my_cdev_exit(void) {
    device_destroy(my_class, dev_num);
    class_destroy(my_class);

    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "baquynh: Module unloaded, all devices remove\n");
}

module_init(my_cdev_init);
module_exit(my_cdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Troc");
MODULE_DESCRIPTION("AAA");