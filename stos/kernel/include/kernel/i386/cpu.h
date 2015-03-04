/*
 * File: cpu.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#ifndef I386_CPU_H_
# define I386_CPU_H_

# include <kernel/arch/types.h>

# include "regs.h"
# include "crx.h"
# include "pm.h"

static inline void enable_irq(void)
{
	__asm__ volatile("sti\n\t");
}

static inline void disable_irq(void)
{
	__asm__ volatile("cli\n\t");
}

# define EFLAGS_CF	(1 << 0)
# define EFLAGS_PF	(1 << 2)
# define EFLAGS_AF	(1 << 4)
# define EFLAGS_ZF	(1 << 6)
# define EFLAGS_SF	(1 << 7)
# define EFLAGS_TF	(1 << 8)
# define EFLAGS_IF	(1 << 9)
# define EFLAGS_DF	(1 << 10)
# define EFLAGS_OF	(1 << 11)
# define EFLAGS_IOPL	(3 << 12)
# define EFLAGS_NT	(1 << 14)
# define EFLAGS_RF	(1 << 16)
# define EFLAGS_VM	(1 << 17)
# define EFLAGS_AC	(1 << 18)
# define EFLAGS_VIF	(1 << 19)
# define EFLAGS_VIP	(1 << 20)
# define EFLAGS_ID	(1 << 21)

static inline u32 get_eflags(void)
{
	u32 res;

	__asm__("pushfl\n\t"
		"popl %0\n\t"
		: "=r" (res));

	return res;
}

static inline void set_eflags(u32 eflags)
{
	__asm__ volatile("pushl %0\n\t"
			 "popfl\n\t"
			 : /* No output */
			 : "r" (eflags)
			 : "cc");
}

static inline int irq_enabled(void)
{
	return get_eflags() & EFLAGS_IF;
}

/**
 * Save the eflags before disabling interrupts.
 * @return: return the old eflags
 */
static inline u32 save_irq(void)
{
	u32 ret = get_eflags();
	disable_irq();
	return ret;
}

/**
 * Restore the previously saved flags, thus enabling IRQ if it was
 * previously enabled.
 */
static inline void restore_irq(u32 old_flags)
{
	set_eflags(old_flags);
}

static inline void invalidate_page(void* vaddr)
{
	__asm__ volatile("invlpg %0\n\t"
			 : /* No output */
			 : "m" (*(uword*)vaddr)
			 : "memory");
}

static inline void hlt(void)
{
	__asm__ volatile("hlt\n\t");
}

#endif /* !I386_CPU_H_ */
