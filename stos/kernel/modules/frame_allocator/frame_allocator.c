/*
 * File: frame_allocator.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: General physical frame allocator
 */

#include <kernel/arch/spinlock.h>
#include <kernel/errno.h>
#include <kernel/frame.h>
#include <kernel/frame.h>
#include <kernel/memory.h>
#include <kernel/module.h>
#include <kernel/stos.h>
#include <kernel/string.h>

struct frame* framestart;
EXPORT_SYMBOL(framestart);
struct frame* frameend;
EXPORT_SYMBOL(frameend);

struct list_node free_frame_lst = LIST_INIT(free_frame_lst);
/*
 * FIXME: some modules use phys_to_frame() to get specific frames
 * instead of using alloc_frame().
 */
static spinlock_t free_frame_lock = SPINLOCK_INIT;

struct frame* alloc_frame(void)
{
	spinlock_lock(&free_frame_lock);
	struct frame* res = NULL;
	if (list_is_empty(&free_frame_lst))
		goto out;

	struct list_node* lres = free_frame_lst.next;
	list_remove(lres);
	res = lentry(lres, struct frame, next);

out:
	spinlock_unlock(&free_frame_lock);
	return res;
}
EXPORT_SYMBOL(alloc_frame);

/*
 * FIXME: Ugly hack, add real allocator later
 */
int alloc_frames(u8 n, struct frame** frames)
{
	spinlock_lock(&free_frame_lock);
	int res = 0;
	phys_t prev, cur;
	struct list_node* lres;
	struct list_node* lres_cur;
	if (list_is_empty(&free_frame_lst))
		goto err;

	lfor_each(&free_frame_lst, lres) {
		lres_cur = lres;
		frames[n - 1] = lentry(lres, struct frame, next);
		prev = frame_to_phys(frames[n - 1]) / PAGE_SIZE;
		s16 i;

		for (i = n - 2; i >= 0; i--) {
			frames[i] = lentry(lres_cur->next, struct frame, next);
			cur = frame_to_phys(frames[i]) / PAGE_SIZE;
			if (--prev != cur)
				break;
			lres_cur = lres_cur->next;
		}
		if (i > 0)
			continue;
		for (u8 j = 0; j < n; j++) {
			lres_cur = lres->next;
			list_remove(lres);
			lres = lres_cur;
		}
		goto out;
	}

err:
	res = -ENOMEM;
out:
	spinlock_unlock(&free_frame_lock);
	return res;
}
EXPORT_SYMBOL(alloc_frames);

static void release_frame(struct refcnt* cnt)
{
	struct frame* frame = container_of(cnt, struct frame, cnt);

	frame->vaddr = JUNK_PTR;

	spinlock_lock(&free_frame_lock);
	list_insert(&free_frame_lst, &frame->next);
	spinlock_unlock(&free_frame_lock);
	refcnt_inc(&frame->cnt);
}

void free_frame(struct frame* frame)
{
	refcnt_dec(&frame->cnt);
}
EXPORT_SYMBOL(free_frame);

static void __init_once init(void)
{
	phys_t last = last_mem_addr();
	u32 nb_frames = last / PAGE_SIZE;
	u32 nb_pages = align2_up(nb_frames * sizeof(struct frame),
				 BIG_PAGE_SIZE) / BIG_PAGE_SIZE;

	/*
	 * The frame array has been allocated at the top of the memory
	 * address space. More specifically, the last BIG_PAGE has been
	 * kept unallocated.
	 */
	framestart = (void*)(align2_down(~(uword)NULL, BIG_PAGE_SIZE)
			     - (nb_pages + 1) * BIG_PAGE_SIZE);

	frameend = framestart + (last / PAGE_SIZE);

	for (struct frame* f = framestart; f < frameend; f++) {
		phys_t frame_nb = frame_to_phys(f);

		switch (get_addr_type(frame_nb)) {
		case AVAILABLE:
			f->vaddr = NULL;
			f->type = PAGE_OTHER;
			list_init(&f->next);
			list_insert(&free_frame_lst, &f->next);
			refcnt_init(&f->cnt, release_frame);
			break;
		case DEVICE_RESERVED:
		default:
			list_init(&f->next);
			break;
		}
	}
}

MODINFO {
	module_name("frame_allocator"),
	module_init_once(init),
	module_type(M_FRAME_ALLOCATOR),
};
