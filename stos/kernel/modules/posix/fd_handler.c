#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/fd_handler.h>
#include <kernel/stos.h>

long __syscall sys_fcntl(int fd, int cmd, void* arg)
{
	if (!is_valid_fd(fd))
		return -EBADF;

	struct file* f = file_get(fd);

	int ret;

	int fd2;
	switch (cmd) {
	case F_DUPFD:
		refcnt_inc(&f->f_refcnt);
		ret = file_install(f, (uword)arg);
		if (ret < 0)
			refcnt_dec(&f->f_refcnt);
		break;
	case F_DUP2FD:
		fd2 = (uword)arg;
		if (fd2 >= get_current()->fds->max_fds) {
			ret = -EBADF;
		} else {
			if (fd2 != fd) {
				if (is_valid_fd(fd2))
					file_release(fd2);
				refcnt_inc(&f->f_refcnt);
				spinlock_lock(&get_current()->fds->files_lock);
				get_current()->fds->files[fd2] = f;
				spinlock_unlock(&get_current()->fds->files_lock);
			}
			ret = fd2;
		}
		break;
	case F_GETFD:
		ret = f->f_flags & FD_CLOEXEC;
		break;
	case F_SETFD:
		ret = 0;
		f->f_flags = (f->f_flags & ~FD_CLOEXEC) | (uword)arg;
		break;
	case F_GETFL:
		ret = f->f_flags & ~FD_CLOEXEC;
		break;
	case F_SETFL:
		f->f_flags = (uword)arg | (f->f_flags & FD_CLOEXEC);
		ret = 0;
		break;
	default:
		ret = -EINVAL;
	}
	refcnt_dec(&f->f_refcnt);
	return ret;
}

