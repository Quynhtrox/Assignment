#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEVICE_NAME      "boquynhdev"

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;

static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "BoQuynh: Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "BoQuynh: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
};

static int __init my_cdev_init(void) {
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev_num, 1);

    my_class = class_create("TROC");
    device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    
    printk(KERN_INFO "BoQuynh: Module loaded, device created at /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit my_cdev_exit(void) {
    device_destroy(my_class, dev_num);
    class_destroy(my_class);

    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "BoQuynh: Module unloaded\n");
}

module_init(my_cdev_init);
module_exit(my_cdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Troc");
MODULE_DESCRIPTION("Implement open and release file operations.");