/*
 * File: mkdir.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement mkdir.
 */

#include <unistd.h>
#include <fcntl.h>

/*
 * In stos, mkdir can be emulated with open, so use it instead of creating
 * another syscall.
 */
int mkdir(const char* pathname, mode_t mode)
{
	int fd = open(pathname, O_CREAT | O_DIRECTORY | O_EXCL, mode);
	if (fd < 0)
		return fd;
	close(fd);
	return 0;
}
