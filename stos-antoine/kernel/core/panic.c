/*
 * File: panic.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implements the panic function
 */

#include <kernel/arch/cpu.h>
#include <kernel/klog.h>
#include <kernel/module.h>
#include <kernel/panic.h>
#include <kernel/arch/pvpanic.h>

/*
 * Prevent recursive panic.
 */
static int has_panicked;

void panic(const char* msg)
{
	if (has_panicked)
		goto loop;

	has_panicked = 1;

	disable_irq();
	klog("Kernel panic: %s\n", msg);

	print_back_trace();
	set_qemu_panicked_state();

loop:
	for (;;)
		continue;
}
EXPORT_SYMBOL(panic);
