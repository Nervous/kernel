#ifndef PAGE_H_
# define PAGE_H_

# include <kernel/arch/page.h>
# include <kernel/frame.h>
# include <kernel/types.h>

/* Find n consecutive free virtual pages and return their address */
void* alloc_pages(struct frame* pdbr, size_t n);

/* Map n physical pages to virtual pages */
int map_pages(struct frame* pdbr, void* vaddr, struct frame* frames[],
	      size_t n, int flags);

void unmap_pages(struct frame* pdbr, void* vaddr, int n);

void* map_io(phys_t ioaddr, size_t len);

#endif /* !PAGE_H_ */
