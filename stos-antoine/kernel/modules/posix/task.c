#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/mem.h>
#include <kernel/page.h>
#include <kernel/syscall.h>

long __syscall sys_waitpid(pid_t pid, int* status, int options)
{
	if (!get_current()->oldest_son)
		return -ECHILD;

	struct sleep_queue q;
	init_sleep_queue(&q);

	pid_t res;
	struct task* dead_son;
	if (pid == -1) {
		/*
		 * Waitpid(-1), means that we should wait on all sons.
		 */
		sleep_on(&q, !list_is_empty(&get_current()->zombies));

		spinlock_lock(&get_current()->zombies_lock);
		struct list_node* l = get_current()->zombies.next;
		list_remove(l);
		spinlock_unlock(&get_current()->zombies_lock);
		dead_son = lentry(l, struct task, tasks);

		assert(dead_son->state == TASK_ZOMBIE);

		res = dead_son->pid;
	} else {
		if (get_current()->oldest_son->pid == pid) {
			dead_son = get_current()->oldest_son;
		} else {
			lfor_each_entry(&get_current()->oldest_son->brothers,
					dead_son,
					brothers) {

				if (dead_son->pid == pid)
					break;

			}
			if (dead_son->pid == pid)
				return -ECHILD;
		}
		sleep_on(&q, dead_son->state == TASK_ZOMBIE);

		spinlock_lock(&get_current()->zombies_lock);
		list_remove(&dead_son->tasks);
		spinlock_unlock(&get_current()->zombies_lock);

		res = pid;
	}

	if (get_current()->oldest_son == dead_son) {
		if (list_is_empty(&dead_son->brothers)) {
			get_current()->oldest_son = NULL;
		} else {
			struct task* bro;
			bro = lentry(dead_son->brothers.next, struct task, brothers);

			get_current()->oldest_son = bro;
		}
	}
	list_remove(&dead_son->brothers);

	release_mem(dead_son->mem);
	kfree(dead_son->mem, sizeof(struct mem));

	spinlock_lock(&dead_son->fds->files_lock);
	for (int i = 0; i < dead_son->fds->max_fds; i++)
		if (dead_son->fds->files[i])
			refcnt_dec(&dead_son->fds->files[i]->f_refcnt);
	/* FIXME: ->fds should be refcounted */
	kfree(dead_son->fds->files, dead_son->fds->max_fds * sizeof(struct file*));
	spinlock_unlock(&dead_son->fds->files_lock);
	kfree(dead_son->fds, sizeof(struct filedesc));

	kfree(dead_son->fs, sizeof (struct fs));

	kfree(dead_son->sched_attr, sizeof(struct sched_attr));

	unmap_pages(get_current()->mem->page_directory,
		    dead_son->mem->page_directory->vaddr, 1);
	unmap_pages(get_current()->mem->page_directory, dead_son, 2);
	/*
	 * TODO:
	 * What about the sons of dead_son? In a UNIX system, those
	 * should be attached to init, if this is done, the ->brothers
	 * will have to be protected by a lock.
	 */
	/*
	 * TODO:
	 * Fill *status and look at the options parameter
	 */
	return res;
}

long __syscall sys_exit(int status)
{
	kill_task(get_current());

	schedule();

	/* Won't be reach. */
	return 0;
}

long __syscall sys_getpid(void)
{
	return get_current()->pid;
}

long __syscall sys_getppid(void)
{
	return get_current()->father->pid;
}

long __syscall sys_getuid(void)
{
	return get_current()->uid;
}

long __syscall sys_getgid(void)
{
	return get_current()->gid;
}
