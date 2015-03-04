#ifndef MSR_H_
# define MSR_H_

# include <kernel/types.h>

static inline u64 get_msr(uword msr)
{
	union {
		u64 res;
		struct {
			u32 lo;
			u32 hi;
		} s;
	} u;

	__asm__("rdmsr\n"
		: "=d" (u.s.hi), "=a" (u.s.lo)
		: "c" (msr));

	return u.res;
}

static inline void set_msr(uword msr, u64 value)
{
	union {
		u64 value;
		struct {
			u32 lo;
			u32 hi;
		} s;
	} u;

	u.value = value;

	__asm__ volatile("wrmsr\n"
			 : /* No output */
			 : "c" (msr), "d" (u.s.hi), "a" (u.s.lo));
}

#endif /* !MSR_H_ */
