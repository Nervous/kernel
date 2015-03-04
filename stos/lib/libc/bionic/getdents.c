/*
 * File: getdents.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Simulate getdents with a read.
 */

#include <fcntl.h>
#include <dirent.h>

int getdents(unsigned int fd, struct dirent* dir, unsigned int size)
{
	return read(fd, dir, size);
}
