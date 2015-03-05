#include <kernel/arch/types.h>
#include <kernel/errno.h>
#include <kernel/fd_handler.h>
#include <kernel/stos.h>
#include <kernel/vfs.h>

off_t __syscall sys_lseek(int fd, off_t offset, int whence)
{
	if (!is_valid_fd(fd))
		return -EBADF;

	struct file* f = file_get(fd);

	off_t ret = vfs_lseek(f, offset, whence);
	refcnt_dec(&f->f_refcnt);
	return ret;
}
