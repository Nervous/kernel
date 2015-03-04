#ifndef I386_CURRENT_H_
# define I386_CURRENT_H_

# include <kernel/types.h>
# include <kernel/task.h>
# include <kernel/stos.h>

static inline struct task* get_current(void)
{
	uword esp;
	__asm__ __volatile("mov %%esp, %0" : "=&r" (esp));
	return  (struct task*)align2_down(esp, KSTACK_SIZE);
}

#endif /* !I386_CURRENT_H_ */
