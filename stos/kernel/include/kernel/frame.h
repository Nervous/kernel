#ifndef FRAME_H_
# define FRAME_H_

# include <kernel/arch/page.h>
# include <kernel/list.h>
# include <kernel/memory.h>
# include <kernel/refcnt.h>

/*
 * Describe the use of the frame, useful for debug
 */
enum frame_type {
	PAGE_OTHER = 0,
	PAGE_DIR,
	PAGE_TABLE
};

struct frame {
	enum frame_type type;
	void* vaddr;
	struct list_node next;
	/*
	 * A list to page table pointing to this frame may be useful for COW,
	 * and may be added in the future.
	 */

	struct refcnt cnt;
};

/*
 * The frame are disposed in an array begining at framestart address.
 * This simplify access to frame from page number.
 */
extern struct frame* framestart;
extern struct frame* frameend;

struct frame* alloc_frame(void);
void free_frame(struct frame* frame);

/*
 * Allocate n phycically contiguous frames
 */
int alloc_frames(u8 n, struct frame** frames);

static inline phys_t frame_to_phys(struct frame* f)
{
	assert(f >= framestart && f <= frameend);
	return ((phys_t)(f - framestart) << PAGE_SHIFT);
}

static inline struct frame* phys_to_frame(phys_t addr)
{
	assert((addr & PAGE_MASK) == addr);
	return &framestart[addr >> PAGE_SHIFT];
}

#endif /* !FRAME_H_ */
