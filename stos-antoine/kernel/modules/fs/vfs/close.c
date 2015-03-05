#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/vfs.h>

#include <kernel/fd_handler.h>

void default_release(struct refcnt* ref)
{
	struct file* f;
	f = container_of(ref, struct file, f_refcnt);

	/*
	 * default_release should only be used if there is no fs_data.
	 */
	assert(!f->f_fs_data);
	kfree(f, sizeof(struct file));
}
EXPORT_SYMBOL(default_release);

void vfs_close(struct file* f)
{
	refcnt_dec(&f->f_refcnt);
}
EXPORT_SYMBOL(vfs_close);
