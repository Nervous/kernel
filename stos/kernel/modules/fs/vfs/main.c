#include <kernel/arch/current.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/string.h>
#include <kernel/syscall.h>
#include <kernel/vfs.h>

static void __init_once init(void)
{
	get_current()->fs = kmalloc(sizeof(struct fs));
	if (!get_current()->fs)
		panic("Could not init the VFS");
	memset(get_current()->fs, 0, sizeof(struct fs));

	if (install_fd_table(get_current(), NR_OPEN) < 0)
		panic("Could not init the VFS");
}

MODINFO {
	module_name("vfs"),
	module_init_once(init),
	module_type(M_VFS),
	module_deps(M_KMALLOC | M_TASK)
};
