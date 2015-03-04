#ifndef MODULE_LOADER_H_
# define MODULE_LOADER_H_

# include <kernel/elf.h>
# include <kernel/module.h>
# include <kernel/list.h>

/*
 * Describe a section relocation.
 * This will also be stored in ->sh_addr.
 */
struct reloc {
	/* Store the virtual address of the relocation */
	void* reloc_vaddr;
	/*
	 * Store the address where it's logically located
	 * eg in x86: %gs:reloc_laddr
	 */
	void* reloc_laddr;
};

/*
 * Describe a section in memory.
 */
struct section {
	struct reloc* reloc;
	size_t len;
	char *name;
};

/*
 * Describe a loaded module in memory.
 */
struct module {
	struct modinfo info;

	int nb_section;
	struct section* sections;

	/* List of modules that depend on this one. */
	struct list_node* dep_modules; /* List of mod_dep */

	/* FIXME: is it useful ? */
	/* List of modules on which this one depends. */
	struct list_node* deps; /* List of mod_dep */

	/* Lists of all modules. */
	struct list_node mods;
};

/**
 * Iterate on all modules.
 * @iter: the function to be called for each module.
 */
void modules_iter(void (*iter)(struct module* m));

struct mod_dep {
	struct module* mod;
	struct list_node next_dep;
};

void init_module_loader(void);

/*
 * Dynamically load the module
 */
int load_module(Elf_Ehdr* elf);

/**
 * Load the modules in order.
 * @modules: Array of elf files.
 * @mod_nb: Size of the @modules array.
 * @return: -errno on error, 0 otherwise.
 */
int load_modules(Elf_Ehdr** modules, u32 mod_nb);

/*
 * The load handler will load the section and will return the relocation
 * information.
 */
int add_section_loader(const char* name,
		       struct reloc* (*load)(Elf_Ehdr* elf, Elf_Shdr* shdr));
struct reloc* default_load_section(Elf_Ehdr* elf, Elf_Shdr* shdr);

int register_pre_init_module_handler(int (*handler)(struct module* mod));
int register_post_init_module_handler(int (*handler)(struct module* mod));

#endif /* !MODULE_LOADER_H_ */
