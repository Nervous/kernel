/*
 * File: kmsg.c
 *
 * Description: interface for klog_console
 */
#include <kernel/device.h>
#include <kernel/errno.h>
#include <kernel/klog.h>
#include <kernel/klog_console.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/user.h>

static int kmsg_open(struct inode* i, struct file* f)
{
	/*
	 * We use f->f_fs_data as file offset since we only want a read index
	 * and not a write index (f->f_offset is modified by vfs_write).
	 */
	f->f_fs_data = kmalloc(sizeof(off_t));
	if (!f->f_fs_data)
		return -ENOMEM;
	*(off_t*)f->f_fs_data = 0;
	return 0;
}

static ssize_t kmsg_read(struct file* file, char* buffer, size_t count)
{
	char* buf = kmalloc(count);
	if (!buf)
		return -ENOMEM;

	size_t length = klog_console_read(buf, *(off_t*)file->f_fs_data, count);
	if (write_user_buf(buffer, buf, length) < 0)
		length = -EINVAL;

	*(off_t*)file->f_fs_data += length;

	kfree(buf, count);
	return length;
}

static ssize_t kmsg_write(struct file* file, const char* buffer, size_t count)
{
	ssize_t ret = count;
	char* tmp = kmalloc(count + 1);
	if (!tmp)
		return -ENOMEM;

	if (copy_user_buf(tmp, buffer, count) < 0) {
		ret = -EINVAL;
		goto err;
	}

	tmp[count] = '\0';
	klog("%s\n", tmp);

err:
	kfree(tmp, count + 1);
	return ret;
}

static void kmsg_release(struct refcnt* ref)
{
	struct file* f;
	f = container_of(ref, struct file, f_refcnt);

	kfree(f->f_fs_data, sizeof(off_t));
	kfree(f, sizeof(struct file));
}

static struct file_ops kmsg_f_ops = {
	.open = kmsg_open,
	.read = kmsg_read,
	.write = kmsg_write,
	.lseek = NULL,
	.release = kmsg_release
};

static void __init_once kmsg_init(void)
{
	register_device(NULL, "kmsg", &kmsg_f_ops, NULL);
}

MODINFO {
	module_name("kmsg"),
	module_init_once(kmsg_init),
	module_deps(M_DEVFS | M_KMALLOC),
};
