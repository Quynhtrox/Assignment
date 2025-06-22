#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

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

static void pdev_function_release(struct device *dev) {
    pr_info("[PDEV] device release\n");
}

// Sửa tên cho đồng bộ với driver
static pdev_infor_t troc_pdev_infor = {
    .name = "trocdev0",
    .id = 0,
    .size = 256,
    .perm = RDWR
};

static struct platform_device troc_pdev = {
    .name = "trocdevice",
    .id = -1,
    .dev = {
        .platform_data = &troc_pdev_infor,
        .release = pdev_function_release
    }
};

static int __init troc_platform_device_init(void) {
    platform_device_register(&troc_pdev);
    pr_info("[TROC-PDEV INIT] Device registered: %s\n", troc_pdev_infor.name);
    return 0;
}

static void __exit troc_platform_device_exit(void) {
    platform_device_unregister(&troc_pdev);
    pr_info("[TROC-PDEV EXIT] Device unregistered: %s\n", troc_pdev_infor.name);
}

module_init(troc_platform_device_init);
module_exit(troc_platform_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Troc");
MODULE_DESCRIPTION("Troc's Platform Device Module");
