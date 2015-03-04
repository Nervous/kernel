/*
 * File: kmalloc.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: First fit kernel allocator
 */

#include <kernel/arch/spinlock.h>
#include <kernel/arch/types.h>
#include <kernel/early_alloc.h>
#include <kernel/kmalloc.h>
#include <kernel/list.h>
#include <kernel/memory.h>
#include <kernel/module.h>
#include <kernel/panic.h>
#include <kernel/task.h>

struct mem_blk {
	struct mem_blk* this;
	size_t blk_size;
	size_t size;

	struct list_node next_blk;
};

struct mem_manager {
	void* brk;
	void* max_brk;

	struct list_node first_blk;
	spinlock_t lock; /* Protect the whole structure and the list. */
};

static struct mem_manager manager;

void* kmalloc(size_t size)
{
	struct mem_blk* blk = NULL;

	if (!size)
		panic("0 size kmalloc");

	size += sizeof(struct mem_blk);

	spinlock_lock(&manager.lock);
	lfor_each_entry(&manager.first_blk, blk, next_blk) {
		if (blk->blk_size >= size) {
			list_remove(&blk->next_blk);
			spinlock_unlock(&manager.lock);
			blk->this = blk;
			blk->size = size;
			return blk + 1;
		}
	}

	blk = manager.brk;
	if (manager.brk + size > manager.max_brk)
		panic("kmalloc: memory exhausted\n");
	manager.brk += size;
	spinlock_unlock(&manager.lock);
	blk->this = blk;
	blk->size = size;
	blk->blk_size = size;
	list_init(&blk->next_blk);

	return blk + 1;
}
EXPORT_SYMBOL(kmalloc);

void kfree(void* ptr, size_t size)
{
	if (!ptr)
		return;
	struct mem_blk* blk = ptr;
	blk--;

	if (blk->this != blk)
		panic("Bad free");

	assert(blk->size == size + sizeof(struct mem_blk));
	blk->this = JUNK_PTR;

	spinlock_lock(&manager.lock);
	list_insert(&manager.first_blk, &blk->next_blk);
	spinlock_unlock(&manager.lock);
}
EXPORT_SYMBOL(kfree);

static void __init_once init(void)
{
	manager.brk = get_early_brk();
	manager.max_brk = (void*)(0xC0400000 - KSTACK_SIZE);
	spinlock_init(&manager.lock);

	list_init(&manager.first_blk);

	/* Replace early_alloc allocator */
	kmalloc_ptr = kmalloc;
	kfree_ptr = kfree;
}

MODINFO {
	module_name("kmalloc"),
	module_init_once(init),
	module_type(M_KMALLOC)
};
