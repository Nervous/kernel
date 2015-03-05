#include <kernel/device.h>
#include <kernel/module.h>
#include <kernel/vfs.h>


static ssize_t null_read(struct file* f, char* buf, size_t size)
{
	return 0;
}

static ssize_t null_write(struct file* f, const char* buf, size_t size)
{
	return size;
}

static off_t null_lseek(struct file* f, off_t offset, int whence)
{
	return 0;
}


static struct file_ops null_f_ops = {
	.open = default_open,
	.read = null_read,
	.write = null_write,
	.lseek = null_lseek,
	.release = default_release,
};


static void __init_once null_init(void)
{
	register_device(NULL, "null", &null_f_ops, NULL);
}

MODINFO {
	module_name("null"),
	module_init_once(null_init),
	module_deps(M_DEVFS),
};
