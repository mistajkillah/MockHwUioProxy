#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/uio_driver.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/io.h>       // for virt_to_phys()
#include <linux/string.h>   // for memset()

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
};

static struct proxy_uio_dev uio_devs[NUM_DEVICES];
static struct platform_device *pdevs[NUM_DEVICES];

static int proxy_probe(struct platform_device *pdev)
{
    int id = pdev->id;
    struct proxy_uio_dev *dev = &uio_devs[id];

    // ✅ Initialize entire uio_info to zero to avoid NULL dereference
    memset(&dev->uio, 0, sizeof(dev->uio));
    dev->id = id;

    snprintf(dev->uio.name, UIO_MAX_NAME_SIZE, "MockHwProxyUIO%d", id);
    dev->uio.version = "0.1";
    dev->uio.mem[0].addr = virt_to_phys(shared_buffer);
    dev->uio.mem[0].memtype = UIO_MEM_LOGICAL;
    dev->uio.mem[0].size = sizeof(struct shared_mem);

    return uio_register_device(&pdev->dev, &dev->uio);
}

static int proxy_remove(struct platform_device *pdev)
{
    int id = pdev->id;
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
    int i;

    shared_buffer = kzalloc(sizeof(struct shared_mem), GFP_KERNEL);
    if (!shared_buffer)
        return -ENOMEM;

    mutex_init(&shared_buffer->lock);

    for (i = 0; i < NUM_DEVICES; i++) {
        pdevs[i] = platform_device_register_simple(DRIVER_NAME, i, NULL, 0);
        if (IS_ERR(pdevs[i]))
            return PTR_ERR(pdevs[i]);
    }

    return platform_driver_register(&proxy_driver);
}

static void __exit proxy_exit(void)
{
    int i;
    platform_driver_unregister(&proxy_driver);
    for (i = 0; i < NUM_DEVICES; i++)
        platform_device_unregister(pdevs[i]);

    kfree(shared_buffer);
}

module_init(proxy_init);
module_exit(proxy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("UIO proxy driver named MockHwUioProxyDriver");
