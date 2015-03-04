#ifndef MEM_H_
# define MEM_H_

# include <kernel/arch/rwspinlock.h>
# include <kernel/frame.h>
# include <kernel/list.h>
# include <kernel/task.h>

struct mem_zone {
	void* beg;
	void* end;

	/* TODO: Access right */

	struct list_node next;
};

/* Describe memory zone used by a task */
struct mem {
	/* An uword should be better */
	struct frame* page_directory; /* page directory base register */
	struct list_node zones;
	rw_spinlock_t mem_lock;
};

int add_mem_zone(struct task* t, void* start, void* end);

int is_addr_valid(struct task* t, void* addr);

int is_addr_range_valid(struct task* t, const void* start, const void* end);

void* find_free_mem_range(struct task* t, size_t len);

/*
 * Release all the physical/virtual memory used by the struct mem. The mem
 * argument is not released.
 */
void release_mem(struct mem* mem);

#endif /* !MEM_H_ */
