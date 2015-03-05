#ifndef TASK_H_
# define TASK_H_

# include <kernel/arch/regs.h>
# include <kernel/arch/spinlock.h>
# include <kernel/arch/task.h>
# include <kernel/clone.h>
# include <kernel/list.h>
# include <kernel/sched.h>
# include <kernel/stos.h>
# include <kernel/types.h>
# include <kernel/vfs.h>

enum task_state {
	TASK_RUNNING, /* Running on a CPU */
	TASK_WAITING, /* Waiting for CPU time. */
	TASK_SLEEPING, /* Waiting for a data to be ready */
	TASK_IDLING, /* Inside the idle loop. */
	TASK_ZOMBIE
};

struct task {
	volatile enum task_state state;
	pid_t pid;

	uid_t uid;
	gid_t gid;

	/* Double chained list of tasks */
	struct list_node tasks;

	/* Tree of tasks */
	/*
	 * I believe that those three fields doesn't need to be protected
	 * by a lock : a task can only have a new children if it forks,
	 * so there won't be any concurrent access to those fields.
	 */
	struct task* father;
	struct list_node brothers;
	struct task* oldest_son;

	/* Zombies tasks */
	struct list_node zombies;
	spinlock_t zombies_lock;

	uword kernel_sp;
	uword kernel_ip;

	struct regs* regs;

	struct fs* fs;
	struct filedesc* fds;

	struct mem* mem;

	struct sched_attr* sched_attr;
};

/*
 * Use linux trick:
 *	Inside the kernel we could get the current task by applying a
 *	mask to %esp, since every task get its own stack.
 *	Implementation is in <arch/current.h>
 */
union task_union {
	struct task task;
	char kernel_stack[8192];
};

# define KSTACK_SIZE (sizeof(union task_union))

struct task* clone_task(struct task* father, enum clone_flags flags);
void kill_task(struct task* task);

enum task_type {
	KERNEL_TASK,
	USER_TASK
};

void prepare_new_task(struct task* t, void* start_ip, enum task_type type);

/* Should not be called directly, only switch_to_task call it */
void __attribute(regparm(3)) finish_switch_to(struct task* prev, struct task* next);

/* Launch a kernel thread */
struct task* kthread_create(void (*thrd_function)(void*), void* args,
			    enum sched_prio prio);

#endif /* !TASK_H_ */
