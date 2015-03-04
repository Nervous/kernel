/*
 * File: main.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#include <boot/interface.h>

#include <kernel/early_alloc.h>
#include <kernel/klog.h>
#include <kernel/klog_console.h>
#include <kernel/memory.h>
#include <kernel/module_loader.h>
#include <kernel/stos.h>
#include <kernel/string.h>

#include <boot/interface.h>

extern void arch_init(void* archdata);

static struct boot_interface* global_boot_interface = NULL;

struct boot_interface* get_boot_interface()
{
	return global_boot_interface;
}
EXPORT_SYMBOL(get_boot_interface);

void kernel_main(struct boot_interface* boot)
{
	init_early_allocator(boot->first_useable_addr);
	klog_console_init();
	arch_init(boot->archdata);
	init_memory_map(boot->segs);

	global_boot_interface = boot;

	klog("This is stos booting (%s) !\n", ARCH);

	init_module_loader();

	load_modules(boot->mods->modules, boot->mods->mod_nb);

	klog("Loading complete\n");

	while (1)
		continue;
}
