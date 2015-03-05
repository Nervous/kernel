#include <kernel/device.h>
#include <kernel/module.h>
#include <kernel/vfs.h>
#include <kernel/string.h>

ssize_t zero_read(struct file* f, char* buf, size_t s)
{
	memset(buf, 0, s);
	return s;
}

static struct file_ops zero_fops = {
	.open = default_open,
	.read = zero_read,
	.write = NULL,
	.lseek = NULL,
	.release = default_release,
};

static void __init_once zero_init(void)
{
	register_device(NULL, "zero", &zero_fops, NULL);
}

MODINFO {
	module_name("zero"),
	module_init_once(zero_init),
	module_deps(M_DEVFS | M_KMALLOC),
};
