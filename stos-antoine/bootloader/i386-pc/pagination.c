/*
 * File: pagination.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Setup a basic pagination for higher-half kernel.
 */
#include <kernel/arch/cpu.h>
#include <kernel/arch/types.h>
#include <kernel/page.h>
#include <kernel/stos.h>
#include "pagination.h"

static void reserve_seg(struct boot_segments* segs, u64 start, u64 len)
{
	for (u32 i = 0; i < segs->seg_nb; i++) {
		struct address_range* seg = &segs->segments[i];
		if (seg->type != 1)
			continue;

		u64 seg_base = seg->base_addr;
		u64 seg_len = seg->length;

		if (seg_base < start) {
			if (seg_base + seg_len >= start)
				seg->length = start - seg_base;
		} else {
			if (seg_base <= start + len) {
				if (seg_base + seg_len <= start + len) {
					seg->type = 2;
				} else {
					seg->base_addr = start + len;
					seg->length = seg_len - len;
				}
			}
		}
	}
}

void launch_paginate_kernel(void* entry, struct boot_interface* boot)
{
	/*
	 * Clang is a _smart_ compiler, writing to the NULL pointer would
	 * generate the ud2 instruction… Adding the volatile qualifier prevent
	 * it from generating the ud2 instruction.
	 */
	volatile u32* page_dir;

	/* Map [0x0-0x400000] (should be enough for the kernel and the
	 * first modules to be loaded) into [0xC0000000-0xC0400000]
	 */
	page_dir = (void*)0x1000;

	page_dir[0] = (u32)NULL;
	page_dir[0] |= P_PRESENT | P_WRITABLE | P_4M;
	page_dir[768] = page_dir[0];

	set_cr3((u32)page_dir);

	set_cr4(get_cr4() | CR4_PSE);
	set_cr0(get_cr0() | CR0_PG);

	reserve_seg(boot->segs, 0x0, BIG_PAGE_SIZE);

	/*
	 * Find the max physical adress, to allocate enough space for
	 * the frame allocator.
	 */
	u64 max_addr = 0;
	for (u32 i = 0; i < boot->segs->seg_nb; i++) {
		struct address_range* seg = &boot->segs->segments[i];
		u64 seg_base = seg->base_addr;
		u64 seg_len = seg->length;
		u64 seg_high = seg_base + seg_len;

		if (seg->type == 1 && max_addr < seg_high)
			max_addr = seg_high;
	}

	u32 nb_frames = max_addr / PAGE_SIZE;
	u32 needed_big_size = align2_up(nb_frames * sizeof(struct frame),
					BIG_PAGE_SIZE);
	u32 nb_big_pages =  needed_big_size / BIG_PAGE_SIZE;
	u32 needed_real_size = align2_up(nb_frames * sizeof(struct frame),
					 PAGE_SIZE);

	reserve_seg(boot->segs, BIG_PAGE_SIZE, needed_real_size);
	u32 paddr = BIG_PAGE_SIZE;
	for (u32 i = 0; i < nb_big_pages; i++) {
		u32 idx = 1023 - (nb_big_pages - i) - 1;
		page_dir[idx] = paddr | P_PRESENT | P_WRITABLE | P_4M;
		paddr += BIG_PAGE_SIZE;
	}

	__asm__ __volatile__("movl $0xC03FFFFC, %%esp\n\t"
			     "movl $0, %%ebp\n\t"
			     "push %%eax\n\t"
			     "call *%0\n\t"
			     : /* No output */
			     : "c" (entry), "a" (VADDR(boot)));
}
