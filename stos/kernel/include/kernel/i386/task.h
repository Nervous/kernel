/*
 * File: task.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: i386 specific task header
 */

#ifndef I386_TASK_H_
# define I386_TASK_H_

# include <kernel/stos.h>
# include <kernel/types.h>

struct tss {
	u16 prev_task_link, unused1;
	u32 esp0;
	u16 ss0, unused2;
	u32 esp1;
	u16 ss1, unused3;
	u32 esp2;
	u16 ss2, unused4;
	u32 cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	u16 es, unused5;
	u16 cs, unused6;
	u16 ss, unused7;
	u16 ds, unused8;
	u16 fs, unused9;
	u16 gs, unused10;
	u16 ldt_segment_selector, unused11;
	u16 t:1, unused12:15;
	u16 io_map_base_address;
} __packed;

#endif /* !I386_TASK_H_ */
