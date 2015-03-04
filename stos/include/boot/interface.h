#ifndef INTERFACE_H_
# define INTERFACE_H_

# include <boot/memory_map.h>
# include <boot/modules.h>

/*
 * This structure and all the modules/segments are expected to be
 * located just after the kernel.
 *
 * first_useable_addr is expected to be used as the early kernel brk.
 */

struct boot_interface {
	struct boot_segments* segs;
	struct boot_modules* mods;
	void* archdata;
	void* first_useable_addr;
	char* commandline;
	void* initramfs;
};

/**
 * This function is located in kernel/core/main.c
 */
struct boot_interface* get_boot_interface();

#endif /* !INTERFACE_H_ */
