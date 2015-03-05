/*
 * File: early_alloc.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Simple memory allocator used for the module loading.
 *		Appart from the module loader, there is absolutely no
 *		reason to use these.
 */

#ifndef EARLY_ALLOC_H_
# define EARLY_ALLOC_H_

# include <kernel/types.h>

/*
 * Init the allocator
 */
void init_early_allocator(void* addr);

void* early_alloc(size_t size);

void early_free(void* addr, size_t size);

void* early_realloc(void* addr,  size_t size);

void* get_early_brk(void);

/*
 * When set, use this in place of the early_alloc functions
 */
extern void* (*kmalloc_ptr)(size_t size);
extern void (*kfree_ptr)(void* ptr, size_t size);

#endif /* !EARLY_ALLOC_H_ */
