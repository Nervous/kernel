/*
 * File: regs.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Stack organisation after a syscall and interrupts
 */
#ifndef I386_REGS_H_
# define I386_REGS_H_

# include <kernel/types.h>

struct regs {
	u32 ebx;
	u32 ecx;
	u32 edx;
	u32 esi;
	u32 edi;
	u32 ebp;
	u32 fs;
	u32 ds;
	u32 eax;
	u32 eip;
	u32 cs;
	u32 eflags;
	/* Undefined if the previous DPL was 0 */
	u32 esp;
	u32 ss;
};

static inline uword* get_user_stack(struct regs* r)
{
	return &r->esp;
}

struct intr_regs {
	u32 ebx;
	u32 ecx;
	u32 edx;
	u32 esi;
	u32 edi;
	u32 ebp;
	u32 fs;
	u32 ds;
	u32 eax;

	u32 intr_nb;
	u32 error_code;

	u32 eip;
	u32 cs;
	u32 eflags;
	/* Undefined if the previous DPL was 0 */
	u32 esp;
	u32 ss;
};

#endif /* !I386_REGS_H_ */
