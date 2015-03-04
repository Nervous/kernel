/*
 * File: kmalloc.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: kmalloc interface.
 *		kmalloc is allocating virtual memory mapped 1:1 with
 *		physical memory.
 */
#ifndef KMALLOC_H_
# define KMALLOC_H_

# include <kernel/types.h>

void* kmalloc(size_t size);
void kfree(void* ptr, size_t size);

#endif /* !KMALLOC_H_ */
