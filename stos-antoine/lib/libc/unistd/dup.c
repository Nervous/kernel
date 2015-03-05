/*
 * File: dup.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement dup and dup2.
 */

#include <unistd.h>
#include <fcntl.h>

int dup(int fildes)
{
	return fcntl(fildes, F_DUPFD, 0);
}

int dup2(int fildes, int filedes2)
{
	return fcntl(fildes, F_DUP2FD, filedes2);
}
