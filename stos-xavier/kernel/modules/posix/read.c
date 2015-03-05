#include <kernel/errno.h>
#include <kernel/fd_handler.h>
#include <kernel/syscall.h>

long __syscall sys_read(int fd, void* buf, size_t n)
{
	if (!is_valid_fd(fd))
		return -EBADF;
	struct file* f = file_get(fd);

	long ret = (long)vfs_read(f, buf, n);
	refcnt_dec(&f->f_refcnt);
	return ret;
}
