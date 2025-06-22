#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

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
	{ 0xeffbdcfa, "_dev_info" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0xec84909d, "of_property_read_variable_u32_array" },
	{ 0x6102e31b, "_dev_err" },
	{ 0x3ea1b6e4, "__stack_chk_fail" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0xe97beb1a, "__platform_driver_register" },
	{ 0xcc4b6d40, "platform_driver_unregister" },
	{ 0x4b8987a, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Ctroc,troc-sensor");
MODULE_ALIAS("of:N*T*Ctroc,troc-sensorC*");

MODULE_INFO(srcversion, "AA36734EF02FAF166212EEA");
