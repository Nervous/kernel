/*
 * File: btrace.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Print an i386 backtrace.
 */

#include <kernel/klog.h>
#include <kernel/panic.h>
#include <kernel/stos.h>

void print_back_trace(void)
{
	u32 ebp;
	__asm__ __volatile__("movl %%ebp, %0\n\t"
			     : "=&r" (ebp));

	int i = 0;
	while (ebp) {
		u32 old_eip_addr = ebp + 4;
		u32 prev_eip = *(u32*)old_eip_addr;
		if (prev_eip < 0xC0000000)
			return;

		klog("#%d: %X\n", i++, prev_eip);

		ebp = *(u32*)ebp;
	}
}
