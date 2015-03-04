/*
 * File: clock.h
 * Authors: Xavier Deguillard <xavier@lse.epita.fr>
 */
#ifndef CLOCK_H_
# define CLOCK_H_

# include <kernel/types.h>

static inline u64 get_cpu_clock(void)
{
	u32 edx, eax;

	__asm__("rdtsc" : "=d" (edx), "=a" (eax));

	return ((u64)edx << 32) | eax;
}

#endif /* !CLOCK_H_ */
