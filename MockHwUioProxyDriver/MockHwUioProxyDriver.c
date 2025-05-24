#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/uio_driver.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>

#define DRIVER_NAME "MockHwUioProxyDriver"
#define NUM_DEVICES 2
#define SHARED_MEM_SIZE 4096

struct shared_mem {
    char data[SHARED_MEM_SIZE];
    struct mutex lock;
};

struct proxy_uio_dev {
    struct uio_info uio;
    int id;
};

static struct proxy_uio_dev uio_devs[NUM_DEVICES];
static struct platform_device *pdevs[NUM_DEVICES];

static struct shared_mem *shared_buffer_virt;
static dma_addr_t shared_buffer_phys;

static int proxy_probe(struct platform_device *pdev)
{
    int id = pdev->id;
    struct proxy_uio_dev *dev = &uio_devs[id];

    pr_info("[%s] Probe for device %d\n", DRIVER_NAME, id);

    dev->id = id;
    memset(&dev->uio, 0, sizeof(dev->uio));

    if (!shared_buffer_virt) {
        shared_buffer_virt = dma_alloc_coherent(&pdev->dev, SHARED_MEM_SIZE, &shared_buffer_phys, GFP_KERNEL);
        if (!shared_buffer_virt) {
            pr_err("[%s] Failed to allocate DMA coherent memory\n", DRIVER_NAME);
            return -ENOMEM;
        }

        mutex_init(&((struct shared_mem *)shared_buffer_virt)->lock);
        pr_info("[%s] Shared buffer allocated at virt %p (phys 0x%llx)\n",
                DRIVER_NAME, shared_buffer_virt, (unsigned long long)shared_buffer_phys);
    }

    dev->uio.name = kasprintf(GFP_KERNEL, "MockHwProxyUIO%d", id);
    dev->uio.version = "0.1";
    dev->uio.mem[0].addr = shared_buffer_phys;
    dev->uio.mem[0].memtype = UIO_MEM_PHYS;
    dev->uio.mem[0].size = SHARED_MEM_SIZE;
    dev->uio.mem[0].name = "shared_mem";

    pr_info("[%s] Registering UIO device %d at phys 0x%llx\n", DRIVER_NAME, id, (unsigned long long)shared_buffer_phys);

    return uio_register_device(&pdev->dev, &dev->uio);
}

static int proxy_remove(struct platform_device *pdev)
{
    int id = pdev->id;
    pr_info("[%s] Removing device %d\n", DRIVER_NAME, id);
    uio_unregister_device(&uio_devs[id].uio);
    kfree(uio_devs[id].uio.name);
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

    pr_info("[%s] Initializing driver\n", DRIVER_NAME);

    ret = platform_driver_register(&proxy_driver);
    if (ret) {
        pr_err("[%s] Failed to register platform driver\n", DRIVER_NAME);
        return ret;
    }

    for (i = 0; i < NUM_DEVICES; i++) {
        pr_info("[%s] Registering platform device %d\n", DRIVER_NAME, i);
        pdevs[i] = platform_device_register_simple(DRIVER_NAME, i, NULL, 0);
        if (IS_ERR(pdevs[i])) {
            pr_err("[%s] Failed to register platform device %d\n", DRIVER_NAME, i);
            while (--i >= 0)
                platform_device_unregister(pdevs[i]);
            platform_driver_unregister(&proxy_driver);
            return PTR_ERR(pdevs[i]);
        }
    }

    return 0;
}

static void __exit proxy_exit(void)
{
    int i;
    pr_info("[%s] Exiting driver\n", DRIVER_NAME);

    for (i = 0; i < NUM_DEVICES; i++) {
        if (pdevs[i])
            platform_device_unregister(pdevs[i]);
    }
    platform_driver_unregister(&proxy_driver);

    if (shared_buffer_virt)
        dma_free_coherent(NULL, SHARED_MEM_SIZE, shared_buffer_virt, shared_buffer_phys);
}

module_init(proxy_init);
module_exit(proxy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MirageInfinite-John Jackson");
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("UIO proxy driver named MockHwUioProxyDriver");
