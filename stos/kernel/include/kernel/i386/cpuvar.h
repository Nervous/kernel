#ifndef CPUVAR_H_
# define CPUVAR_H_

# include <kernel/stos.h>

/*
 * FIXME: find a way to "pin" a task to a specific CPU when
 * reading/writing from a cpuvar. It will avoid the locks and the
 * premption disable.
 */

/*
 * FIXME: cpuvar_read8/16/32 is better imho.
 */
# define cpuvar_read(Var)			\
	({					\
		__typeof__(Var) __res;		\
		__asm__("movl %%fs:%P1, %0"	\
			: "=&r" (__res)		\
			: "m" (Var));		\
		__res;				\
	 })

# define cpuvar_write(Var, Value)		\
	do {					\
		__asm__("movl %1, %%fs:%P0"	\
			: "=m" (Var)		\
			: "r" (Value));		\
	} while (0)

#endif /* !CPUVAR_H_ */
