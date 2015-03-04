/*
 * File: wait.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement wait with the help of waitpid.
 */

#include <sys/wait.h>
#include <stddef.h>

pid_t wait(int* status)
{
	return waitpid((pid_t)-1, status, 0);
}
