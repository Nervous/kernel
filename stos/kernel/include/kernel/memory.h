/*
 * File: memory.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */
#ifndef MEMORY_H_
# define MEMORY_H_

# include <boot/memory_map.h>
# include <kernel/types.h>

typedef uword phys_t;

enum mem_type {
	AVAILABLE,
	DEVICE_RESERVED
};

void init_memory_map(struct boot_segments* segs);

void add_mem_segment(phys_t base_addr, u64 len, enum mem_type type);

/* Return 1 if the reservation passed, 0 otherwise */
int reserve_mem_segment(phys_t addr, u64 len);

phys_t last_mem_addr(void);

enum mem_type get_addr_type(phys_t addr);

#endif /* !MEMORY_H_ */
