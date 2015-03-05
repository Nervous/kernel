/*
 * File: interrupts.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Interruption facility header.
 */
#ifndef INTERRUPTS_H_
# define INTERRUPTS_H_

# include <kernel/arch/regs.h>

enum interrupt_flag {
	TRAP_GATE = 1, /* IRQ are enabled during the interruption */
	USER_GATE = 2, /* The user can call it */
	WRAPPED_GATE = 4 /* Should the handler be wrapped? */
};

typedef void (*isr_handler)(struct intr_regs* regs);

void interrupt_add_isr(isr_handler handler, int n, enum interrupt_flag flags);

void interrupt_rm_isr(int n);

#endif /* !INTERRUPTS_H_ */
