/*
 * File: memory.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Handle the available memory
 */
#include <kernel/arch/page.h>
#include <kernel/arch/spinlock.h>
#include <kernel/early_alloc.h>
#include <kernel/klog.h>
#include <kernel/memory.h>
#include <kernel/module.h>
#include <kernel/list.h>
#include <kernel/stos.h>

struct mem_segment {
	phys_t base_addr;
	phys_t end_addr;

	enum mem_type type;

	struct list_node next_seg;
};
static struct list_node segments = LIST_INIT(segments);
static spinlock_t segments_lock = SPINLOCK_INIT;

void add_mem_segment(phys_t base_addr, u64 len, enum mem_type type)
{
	struct mem_segment* seg = early_alloc(sizeof(struct mem_segment));
	seg->base_addr = base_addr;
	seg->end_addr = base_addr + len;
	seg->type = type;
	list_init(&seg->next_seg);
	spinlock_lock(&segments_lock);
	list_insert(&segments, &seg->next_seg);
	spinlock_unlock(&segments_lock);
}

int reserve_mem_segment(phys_t addr, u64 len)
{
	struct mem_segment* seg;
	spinlock_lock(&segments_lock);
	lfor_each_entry(&segments, seg, next_seg) {
		if (addr >= seg->base_addr && addr + len <= seg->end_addr) {
			struct mem_segment* after;
			after = early_alloc(sizeof(struct mem_segment));
			after->base_addr = addr + len;
			after->end_addr = seg->end_addr;
			after->type = seg->type;
			list_init(&after->next_seg);
			seg->end_addr = addr;
			list_insert(&seg->next_seg, &after->next_seg);
			if (seg->base_addr == addr)
				list_remove(&seg->next_seg);
			spinlock_unlock(&segments_lock);
			return 1;
		}
	}
	spinlock_unlock(&segments_lock);
	return 0;
}

phys_t last_mem_addr(void)
{
	struct mem_segment* seg;
	phys_t res = 0;
	spinlock_lock(&segments_lock);
	lfor_each_entry(&segments, seg, next_seg)
		if (seg->end_addr > res && seg->type == AVAILABLE)
			res = seg->end_addr;
	spinlock_unlock(&segments_lock);
	return res;
}
EXPORT_SYMBOL(last_mem_addr);

enum mem_type get_addr_type(phys_t addr)
{
	assert((((uword)addr) & PAGE_MASK) == (uword)addr);
	struct mem_segment* seg;
	spinlock_lock(&segments_lock);
	lfor_each_entry(&segments, seg, next_seg)
		if (addr >= seg->base_addr && addr <= seg->end_addr) {
			spinlock_unlock(&segments_lock);
			return seg->type;
		}
	spinlock_unlock(&segments_lock);
	return DEVICE_RESERVED;
}
EXPORT_SYMBOL(get_addr_type);

void init_memory_map(struct boot_segments* segments)
{
	klog("Boot memory map:\n");
	for (u32 i = 0; i < segments->seg_nb; i++) {
		struct address_range* seg = &segments->segments[i];
		phys_t base_addr = seg->base_addr;
		u32 segment_len = seg->length;
		phys_t end_addr = base_addr + segment_len;
		const char* disp;
		enum mem_type type;
		switch (seg->type) {
		case MEMORY_AVAILABLE:
			type = AVAILABLE;
			disp = "Available";
			break;
		case MEMORY_RESERVED:
			type = DEVICE_RESERVED;
			disp = "Reserved";
			break;
		default:
			continue;
		}
		klog("  %X - %X: %s\n", base_addr, end_addr, disp);
		add_mem_segment(base_addr, segment_len, type);

	}
}
