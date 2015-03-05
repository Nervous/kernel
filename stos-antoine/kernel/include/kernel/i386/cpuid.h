#ifndef CPUID_H_
# define CPUID_H_

# include <kernel/types.h>

static inline void cpuid(uword* eax, uword* ebx, uword* ecx, uword* edx)
{
	__asm__("cpuid"
		: "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
		: "0" (*eax));
}

#endif /* !CPUID_H_ */
