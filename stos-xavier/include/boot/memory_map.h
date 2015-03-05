#ifndef MEMORY_MAP_H_
# define MEMORY_MAP_H_

# include <kernel/types.h>

struct address_range {
	u64 base_addr;
	u64 length;
# define MEMORY_AVAILABLE	1
# define MEMORY_RESERVED	2
	u32 type;
};

struct boot_segments {
	u32 seg_nb;
	struct address_range segments[];
};

#endif /* !MEMORY_MAP_H_ */
