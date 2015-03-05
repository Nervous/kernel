/*
 * File: early_alloc.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Simple memory allocator used for the module loading
 */

#include <kernel/early_alloc.h>
#include <kernel/klog.h>
#include <kernel/module.h>
#include <kernel/string.h>
#include <kernel/stos.h>

static void* early_brk = NULL;

void* (*kmalloc_ptr)(size_t size) = NULL;
void (*kfree_ptr)(void* ptr, size_t size) = NULL;
EXPORT_SYMBOL(kmalloc_ptr);
EXPORT_SYMBOL(kfree_ptr);

void init_early_allocator(void* addr)
{
	early_brk = (void*)align2_up((uword)addr, sizeof(uword));
}

void* early_alloc(size_t size)
{
	if (kmalloc_ptr) {
		void* res = kmalloc_ptr(size);
		return res;
	}

	void* res = early_brk;

	early_brk = (char*)early_brk + size;

	return res;
}
EXPORT_SYMBOL(early_alloc);

void early_free(void* addr, size_t size)
{
	if (kfree_ptr && addr > early_brk)
		kfree_ptr(addr, size);
}
EXPORT_SYMBOL(early_free);

void* early_realloc(void* addr, size_t size)
{
	void* res = early_alloc(size);

	memcpy(res, addr, size);

	return res;
}

void* get_early_brk(void)
{
	return early_brk;
}
EXPORT_SYMBOL(get_early_brk);
