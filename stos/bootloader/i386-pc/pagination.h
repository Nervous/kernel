#ifndef PAGINATION_H_
# define PAGINATION_H_

# include <boot/interface.h>

# define PADDR(Addr) ((void*)Addr - 0xC0000000)
# define VADDR(Addr) ((void*)Addr + 0xC0000000)

void launch_paginate_kernel(void* entry, struct boot_interface* boot);

#endif /* !PAGINATION_H_ */
