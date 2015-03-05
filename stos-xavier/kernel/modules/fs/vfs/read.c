#include <kernel/arch/types.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/fd_handler.h>
#include <kernel/module.h>
#include <kernel/syscall.h>
#include <kernel/vfs.h>

ssize_t vfs_read(struct file* f, void* buf, size_t n)
{
	if (!(f->f_flags & O_RDONLY))
		return -EINVAL;

	assert(f->f_ops);
	if (!f->f_ops->read)
		return -EBADF;
	/* TODO: check if buf is in address space */

	/*
	if (f->f_offset + (off_t)n > f->f_inode->i_size)
		n = f->f_inode->i_size - f->f_offset;
		*/

	int ret = f->f_ops->read(f, buf, n);
	if (ret < 0)
		return ret;

	f->f_offset += ret;
	return ret;
}
EXPORT_SYMBOL(vfs_read);
