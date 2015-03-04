#include <kernel/arch/types.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/fd_handler.h>
#include <kernel/module.h>
#include <kernel/vfs.h>

off_t default_lseek(struct file* f, off_t offset, int whence)
{
	switch (whence) {
	case SEEK_CUR:
		f->f_offset += offset;
		break;
	case SEEK_END:
		f->f_offset = f->f_inode->i_size + offset;
		break;
	case SEEK_SET:
		f->f_offset = offset;
		break;
	default:
		return -EINVAL;
	}

	if (f->f_offset > f->f_inode->i_size)
		f->f_offset = f->f_inode->i_size;

	return f->f_offset;
}
EXPORT_SYMBOL(default_lseek);

off_t vfs_lseek(struct file* f, off_t offset, int whence)
{
	/* TODO: handle offset overflow */
	assert(f->f_ops);
	if (!f->f_ops->lseek)
		return -EBADF;
	return f->f_ops->lseek(f, offset, whence);
}
EXPORT_SYMBOL(vfs_lseek);
