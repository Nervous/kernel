#ifndef CPU_TOPOLOGY_H
# define CPU_TOPOLOGY_H

# include <kernel/types.h>

struct lapic {
	u8 id;
};

void iterate_all_online_cpus(void (*fn)(struct lapic* cpu, void *args), void *args);

# define MAX_CPUS	256

#endif /* !CPU_TOPOLOGY_H */
