#include <kernel/errno.h>
#include <kernel/fd_handler.h>
#include <kernel/stos.h>

int __syscall sys_close(int fd)
{
	if (!is_valid_fd(fd))
		return -EBADF;
	file_release(fd);
	return 0;
}
