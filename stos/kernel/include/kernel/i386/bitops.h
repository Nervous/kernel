#ifndef I386_BITOPS_H_
# define I386_BITOPS_H_

# include <kernel/types.h>

static inline int bit_scan(u32 n)
{
	int res;

	__asm__("bsfl %1, %0\n\t"
		: "=&r" (res)
		: "r" (n));

	return res;
}

static inline int bit_scan_reverse(u32 n)
{
	int res;

	__asm__("bsrl %1, %0\n\t"
		: "=&r" (res)
		: "r" (n));

	return res;
}

#endif /* !I386_BITOPS_H_ */
