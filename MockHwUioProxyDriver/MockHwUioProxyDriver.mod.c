#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xfb384d37, "kasprintf" },
	{ 0x4267ef06, "__uio_register_device" },
	{ 0xf7d0bcb, "dma_alloc_attrs" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xf07d9d2a, "platform_device_unregister" },
	{ 0x3d8ee50d, "platform_driver_unregister" },
	{ 0xf18befa3, "dma_free_attrs" },
	{ 0xe4cfdada, "__platform_driver_register" },
	{ 0x5302e052, "platform_device_register_full" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x22bb28ea, "uio_unregister_device" },
	{ 0x37a0cba, "kfree" },
	{ 0xf079b8f9, "module_layout" },
};

MODULE_INFO(depends, "uio");


MODULE_INFO(srcversion, "026ACC182FD4F3992288F42");
