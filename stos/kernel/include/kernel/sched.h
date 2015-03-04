/*
 * File: sched.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Interface for the idle task and the scheduler
 */
#ifndef SCHED_H_
# define SCHED_H_

# include <kernel/arch/spinlock.h>
# include <kernel/list.h>

enum sched_prio {
	RT_PRIO = 0,
	KERNEL_PRIO = 1,
	USER_PRIO = 2,
	IDLE_PRIO = 3
};

# define NB_SCHED_PRIO (IDLE_PRIO + 1)

struct sched_attr {
	enum sched_prio prio;
	u64 running_start;
	union {
		u64 running_time;
		/*
		 * RT Task should be scheduled according to their
		 * deadline
		 */
		u64 deadline;
	};
};

/* Idle function */
void idle(void);

/* Scheduler entry point */
void schedule(void);

struct sleep_queue {
	struct list_node task_list;
	spinlock_t queue_lock; /* Protect the linked list */
};

static inline void init_sleep_queue(struct sleep_queue* q)
{
	list_init(&q->task_list);
	spinlock_init(&q->queue_lock);
}

struct task;

void enqueue_task(struct task* t);

/*
 * Sleep until the condition becomes true.
 */
# define sleep_on(q, condition)			\
	do {					\
		while (!(condition))		\
			__sleep_on(q);		\
	} while (0)

/* Wake up all the process that sleeps on this sleep queue. */
void wake_up(struct sleep_queue* q);

void wake_up_task(struct task* t);

/* Sleep until a wake_up. This is not expected to be used as such. */
void __sleep_on(struct sleep_queue* q);

#endif /* !SCHED_H_ */
