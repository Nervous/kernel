/*
 * File: clone.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement the clone syscall.
 */

#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/syscall.h>
#include <kernel/task.h>

long __syscall sys_clone(enum clone_flags flags)
{
	struct task* t = clone_task(get_current(), flags);
	if (is_err_ptr(t))
		return get_err_ptr(t);

	/* Move this */
	t->regs->eax = 0;

	t->state = TASK_WAITING;
	enqueue_task(t);

	return t->pid;
}
