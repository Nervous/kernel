/*
 * File: crx.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Function and definition relative to cr[01234].
 */

#ifndef I386_CRX_H_
# define I386_CRX_H_

# include <kernel/types.h>

# define CR0_PE		(1 << 0)
# define CR0_MP		(1 << 1)
# define CR0_EM		(1 << 2)
# define CR0_TS		(1 << 3)
# define CR0_ET		(1 << 4)
# define CR0_NE		(1 << 5)
# define CR0_WP		(1 << 16)
# define CR0_AM		(1 << 18)
# define CR0_NW		(1 << 29)
# define CR0_CD		(1 << 30)
# define CR0_PG		(1 << 31)

static inline u32 get_cr0(void)
{
	u32 ret;

	__asm__("movl %%cr0, %0\n\t"
		: "=&r" (ret));

	return ret;
}

static inline void set_cr0(u32 v)
{
	__asm__("movl %0, %%cr0\n\t"
		: /* No output */
		: "r" (v)
		: "memory");
}

/*
 * cr2 is not writable, but will contain the faulting address on page
 * fault
 */
static inline u32 get_cr2(void)
{
	u32 ret;

	__asm__("movl %%cr2, %0\n\t"
		: "=&r" (ret));

	return ret;
}

static inline u32 get_cr3(void)
{
	u32 ret;

	__asm__("movl %%cr3, %0\n\t"
		: "=&r" (ret));

	return ret;
}

static inline void set_cr3(u32 v)
{
	__asm__("movl %0, %%cr3\n\t"
		: /* No output */
		: "r" (v)
		: "memory");
}

# define CR4_VME	(1 << 0)
# define CR4_PVI	(1 << 1)
# define CR4_TSD	(1 << 2)
# define CR4_DE		(1 << 3)
# define CR4_PSE	(1 << 4)
# define CR4_PAE	(1 << 5)
# define CR4_MCE	(1 << 6)
# define CR4_PGE	(1 << 7)
# define CR4_PCE	(1 << 8)
# define CR4_OSFXSR	(1 << 9)
# define CR4_OSXMMEXCPT	(1 << 10)

static inline u32 get_cr4(void)
{
	u32 ret;

	__asm__("movl %%cr4, %0\n\t"
		: "=&r" (ret));

	return ret;
}

static inline void set_cr4(u32 v)
{
	__asm__("movl %0, %%cr4\n\t"
		: /* No output */
		: "r" (v)
		: "memory");
}
#endif /* !CRX_H_ */
