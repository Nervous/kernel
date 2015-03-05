/*
 * File: fork.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement fork() with the help of clone()
 */

#include <sys/types.h>
#include <sched.h>

pid_t fork(void)
{
	return clone(CLONE_FORK);
}
