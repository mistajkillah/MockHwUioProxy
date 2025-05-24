#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/uio_driver.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include <linux/string.h>

#define DRIVER_NAME "MockHwUioProxyDriver"
#define NUM_DEVICES 2
#define SHARED_MEM_SIZE 4096

#ifndef UIO_MAX_NAME_SIZE
#define UIO_MAX_NAME_SIZE 64
#endif

struct shared_mem {
    char data[SHARED_MEM_SIZE];
    struct mutex lock;
};

static struct shared_mem *shared_buffer;

struct proxy_uio_dev {
    struct uio_info uio;
    int id;
    char name_buf[UIO_MAX_NAME_SIZE];
};

static struct proxy_uio_dev uio_devs[NUM_DEVICES];
static struct platform_device *pdevs[NUM_DEVICES];

static int proxy_probe(struct platform_device *pdev)
{
    int id = pdev->id;
    int ret;
    struct proxy_uio_dev *dev = &uio_devs[id];

    printk(KERN_INFO "[MockHwUioProxyDriver] Probe for device %d\n", id);

    memset(&dev->uio, 0, sizeof(dev->uio));
    dev->id = id;

    snprintf(dev->name_buf, sizeof(dev->name_buf), "MockHwProxyUIO%d", id);
    dev->uio.name = dev->name_buf;
    dev->uio.version = "0.1";

    dev->uio.mem[0].name = "shared_mem";
    dev->uio.mem[0].addr = virt_to_phys(shared_buffer);
    dev->uio.mem[0].memtype = UIO_MEM_LOGICAL;
    dev->uio.mem[0].size = sizeof(struct shared_mem);

    dev->uio.mem[1].size = 0;  // End of list

    printk(KERN_INFO "[MockHwUioProxyDriver] Registering UIO device %d at phys 0x%llx\n",
           id, (unsigned long long)dev->uio.mem[0].addr);

    ret = uio_register_device(&pdev->dev, &dev->uio);
    if (ret) {
        printk(KERN_ERR "[MockHwUioProxyDriver] uio_register_device failed for id=%d with error %d\n", id, ret);
    }

    return ret;
}

static int proxy_remove(struct platform_device *pdev)
{
    int id = pdev->id;
    printk(KERN_INFO "[MockHwUioProxyDriver] Removing UIO device %d\n", id);
    uio_unregister_device(&uio_devs[id].uio);
    return 0;
}

static struct platform_driver proxy_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = proxy_probe,
    .remove = proxy_remove,
};

static int __init proxy_init(void)
{
    int i, ret;

    printk(KERN_INFO "[MockHwUioProxyDriver] Initializing driver\n");

    shared_buffer = kzalloc(sizeof(struct shared_mem), GFP_KERNEL);
    if (!shared_buffer) {
        printk(KERN_ERR "[MockHwUioProxyDriver] Failed to allocate shared memory\n");
        return -ENOMEM;
    }

    mutex_init(&shared_buffer->lock);
    printk(KERN_INFO "[MockHwUioProxyDriver] Shared buffer allocated at virt %p (phys %llx)\n",
           shared_buffer, (unsigned long long)virt_to_phys(shared_buffer));

    for (i = 0; i < NUM_DEVICES; i++) {
        printk(KERN_INFO "[MockHwUioProxyDriver] Registering platform device %d\n", i);
        pdevs[i] = platform_device_register_simple(DRIVER_NAME, i, NULL, 0);
        if (IS_ERR(pdevs[i])) {
            ret = PTR_ERR(pdevs[i]);
            printk(KERN_ERR "[MockHwUioProxyDriver] platform_device_register_simple failed for %d: %d\n", i, ret);
            return ret;
        }
    }

    ret = platform_driver_register(&proxy_driver);
    if (ret) {
        printk(KERN_ERR "[MockHwUioProxyDriver] platform_driver_register failed: %d\n", ret);
    }

    return ret;
}

static void __exit proxy_exit(void)
{
    int i;
    printk(KERN_INFO "[MockHwUioProxyDriver] Exiting driver\n");

    platform_driver_unregister(&proxy_driver);

    for (i = 0; i < NUM_DEVICES; i++) {
        if (pdevs[i]) {
            printk(KERN_INFO "[MockHwUioProxyDriver] Unregistering platform device %d\n", i);
            platform_device_unregister(pdevs[i]);
        }
    }

    kfree(shared_buffer);
    printk(KERN_INFO "[MockHwUioProxyDriver] Freed shared buffer\n");
}

module_init(proxy_init);
module_exit(proxy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("UIO proxy driver named MockHwUioProxyDriver");
