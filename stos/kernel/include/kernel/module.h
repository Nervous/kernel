/*
 * File: module.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */
#ifndef MODULE_H_
# define MODULE_H_

# include <kernel/elf.h>
# include <kernel/stos.h>

struct ksym {
	void* addr;
	char* name;
};

/*
 * Filled by the linker at compile time.
 */
extern unsigned char ksym_start[];
extern unsigned char ksym_end[];

/*
 * Use this macros to export a symbol so that it's visible by others
 * modules
 */
# define EXPORT_SYMBOL(Name)						\
	static const struct ksym ksym_ ## Name __ksym_section __used = { \
		.addr = &Name,						\
		.name = #Name						\
	}

# define __ksym_section __section(".ksym")

/*
 * Describe the exported modules APIs
 *
 * A module_type describe an API exported by a module. It can export multiple
 * APIs, for example if a module export kmalloc() and a slab API, its type
 * should be :
 *
 * 	module_type(M_KMALLOC | M_SLAB)
 */
enum module_type {
	M_KMALLOC		= 1ULL << 0,
	M_INTERRUPTS		= 1ULL << 1,
	M_IRQ			= 1ULL << 2,
	M_PAGINATION		= 1ULL << 3,
	M_TIMER			= 1ULL << 4,
	M_SCHED			= 1ULL << 5,
	M_BLOCK			= 1ULL << 6,
	M_VFS			= 1ULL << 7,
	M_IO_SCHED		= 1ULL << 8,
	M_TASK			= 1ULL << 9,
	M_FRAME_ALLOCATOR	= 1ULL << 10,
	M_PAGE_ALLOCATOR	= 1ULL << 11,
	M_LIBRARIES		= 1ULL << 12,
	M_MEMORY_INIT		= 1ULL << 13,
	M_BIN			= 1ULL << 14,
	M_DEVFS			= 1ULL << 15,
	M_SYSCALL		= 1ULL << 16,
	M_CHAR			= 1ULL << 17,
	M_PCI			= 1ULL << 18,
	M_CPUVAR		= 1ULL << 19,
	M_SLAB			= 1ULL << 20,
	M_SMP			= 1ULL << 24,
	M_MP			= 1ULL << 25,
	M_CPU_TOPOLOGY		= 1ULL << 26,
	M_APIC			= 1ULL << 27,
	M_SERIAL		= 1ULL << 28,
	M_COMMANDLINE		= 1ULL << 29,
	M_TEST			= 1ULL << 30,
	M_START_ULAND		= 1ULL << 31,
};

/*
 * Modinfo identify an on-disk module.
 */
struct modinfo {
	char* name;
	int major; /* Major kernel version */
	int minor; /* Minor kernel version */

	u64 mod_type;
	u64 mod_deps;

	void (*init_once)(void);
	void (*init)(void);
	void (*exit_once)(void);
	void (*exit)(void);
};

/**
 * struct modparam - describe a module parameter
 *
 * @name: parameter name
 * @dest: where the parameter's value should be loaded
 */
struct modparam {
	const struct modinfo* modinfo;
	const char* param_name;
	void* param_dest;
};

# define __init __section(".init") __used
# define __init_once __section(".init_once") __used
# define __exit __section(".exit") __used
# define __exit_once __section(".exit_once") __used

# define __modinfo_section __section(".modinfo")
# define __modparam_section __section(".modparam")

# define MODINFO									\
	static const struct modinfo __modinfo __modinfo_section __used __align(4) =

# define module_name(Name)	\
	.name = Name,		\
	.major = MAJOR,		\
	.minor = MINOR

# define module_init(Fn)	\
	.init = Fn

# define module_init_once(Fn)	\
	.init_once = Fn

# define module_exit(Fn)	\
	.exit = Fn

# define module_exit_once(Fn)	\
	.exit_once = Fn

# define module_deps(Deps)	\
	.mod_deps = Deps

# define module_type(Type)	\
	.mod_type = Type

# define module_param(Name, Dest)	\
	static const struct modparam __ ## Name __modparam_section __used __align(4) =	\
	{ &__modinfo, #Name, Dest };

#endif /* !MODULE_H_ */
