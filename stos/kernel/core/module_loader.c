/*
 * File: module_loader.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Handle the dynamic relocation and symbol lookup on the
 *		dynamically loader modules.
 */

#include <kernel/arch/spinlock.h>
#include <kernel/early_alloc.h>
#include <kernel/elf.h>
#include <kernel/errno.h>
#include <kernel/klog.h>
#include <kernel/list.h>
#include <kernel/module_loader.h>
#include <kernel/string.h>
#include <kernel/stos.h>

/*
 * FIXME: Many error paths doesn't free anything, thus leading to memory
 * leaks.
 */

/*
 * List of all loaded modules, sorted by load order
 */
static struct list_node modules = LIST_INIT(modules);
/*
 * Protect the list, and the content of each modules.
 */
static spinlock_t modules_lock = SPINLOCK_INIT;

void modules_iter(void (*iter)(struct module* m))
{
	struct module* mod;
	spinlock_lock(&modules_lock);
	lfor_each_entry(&modules, mod, mods)
		iter(mod);
	spinlock_unlock(&modules_lock);
}
EXPORT_SYMBOL(modules_iter);

struct exported_symbol {
	struct ksym sym;

	struct list_node next_sym;
};
static struct list_node syms = LIST_INIT(syms);
static spinlock_t syms_lock = SPINLOCK_INIT;

struct undefined_symbol {
	char* name;
	/* Store the relocation of the symbol. */
	struct reloc* section_reloc;
	uword reloc_value;
	Elf_RelM* rel;

	struct list_node next_undefined;
};
static struct list_node undefined = LIST_INIT(undefined);
static spinlock_t undefined_lock = SPINLOCK_INIT;

static int export_symbol(struct ksym* sym)
{
	struct exported_symbol* esym;

	esym = early_alloc(sizeof(struct exported_symbol));
	if (!esym)
		return -ENOMEM;

	memcpy(&esym->sym, sym, sizeof(struct ksym));

	list_init(&esym->next_sym);
	spinlock_lock(&syms_lock);
	list_insert(&syms, &esym->next_sym);
	spinlock_unlock(&syms_lock);

	struct undefined_symbol* und_sym;
	spinlock_lock(&undefined_lock);
	lfor_each_entry_safe(&undefined, und_sym, next_undefined) {
		if (!strcmp(und_sym->name, sym->name)) {
			*(uword*)und_sym->section_reloc->reloc_vaddr = und_sym->reloc_value;
			assign_value(und_sym->section_reloc->reloc_vaddr,
				     und_sym->section_reloc->reloc_laddr,
				     (Elf_Addr)sym->addr, und_sym->rel);
			list_remove(&und_sym->next_undefined);
			early_free(und_sym->section_reloc, sizeof(struct reloc));
			early_free(und_sym, sizeof(struct undefined_symbol));
		}
	}
	spinlock_unlock(&undefined_lock);
	return 0;
}

static void export_kernel_syms(void)
{
	struct ksym* sym;

	for (sym = (void*)ksym_start; sym < (struct ksym*)ksym_end; sym++)
		if (export_symbol(sym) < 0)
			panic("Could not export kernel syms.");
}

static void* symbol_lookup(char* sym_name)
{
	struct exported_symbol* tmp;

	spinlock_lock(&syms_lock);
	lfor_each_entry(&syms, tmp, next_sym)
		if (!strcmp(tmp->sym.name, sym_name)) {
			spinlock_unlock(&syms_lock);
			return tmp->sym.addr;
		}
	spinlock_unlock(&syms_lock);
	return NULL;
}

static inline Elf_Shdr* get_section(Elf_Ehdr* elf, int nb)
{
	Elf_Shdr* shdr = (Elf_Shdr*)((char*)elf + elf->e_shoff);

	if (nb >= elf->e_shnum)
		return NULL;
	return shdr + nb;
}

static inline char* get_name(Elf_Ehdr* elf, Elf_Half strdx, Elf_Half idx)
{
	Elf_Shdr* strsh = get_section(elf, strdx);

	return (char*)elf + strsh->sh_offset + idx;
}

static inline void* get_vaddr(Elf_Shdr* shdr)
{
	struct reloc* reloc = (void*)shdr->sh_addr;
	return reloc->reloc_vaddr;
}

static inline void* get_laddr(Elf_Shdr* shdr)
{
	struct reloc* reloc = (void*)shdr->sh_addr;
	return reloc->reloc_laddr;
}

/*
 * Return the section name, at index idx
 */
static inline char* section_name(Elf_Ehdr* elf, Elf_Half idx)
{
	return get_name(elf, elf->e_shstrndx, idx);
}

static inline void* get_section_data(Elf_Ehdr* elf, Elf_Shdr* s)
{
	return (char*)elf + s->sh_offset;
}

/*
 * Return the named section
 */
static Elf_Shdr* get_named_section(Elf_Ehdr* elf, char* name)
{
	Elf_Shdr* res = get_section(elf, 1);

	for (int i = 1; i < elf->e_shnum; i++, res++)
		if (!strcmp(section_name(elf, res->sh_name), name))
			return res;
	return NULL;
}

static void panic_undefined(void)
{
	/*
	 * Ideally we may get the previous stack frame and print the undefined
	 * symbol that caused the panic.
	 */
	panic("Panic on undefined symbol call.");
}

static int undef_assign_value(void* reloc_vaddr, void* reloc_laddr, char* name,
			      Elf_RelM* rel)
{
	struct undefined_symbol* und;
	und = early_alloc(sizeof(struct undefined_symbol));
	if (!und)
		return -ENOMEM;

	und->section_reloc = early_alloc(sizeof(struct reloc));
	if (!und->section_reloc)
		return -ENOMEM;

	und->section_reloc->reloc_vaddr = reloc_vaddr;
	und->section_reloc->reloc_laddr = reloc_laddr;
	und->reloc_value = *(uword*)reloc_vaddr;
	und->name = name;
	und->rel = rel;
	list_init(&und->next_undefined);

	spinlock_lock(&undefined_lock);
	list_insert(&undefined, &und->next_undefined);
	spinlock_unlock(&undefined_lock);

	assign_value(reloc_vaddr, reloc_laddr, (Elf_Addr)panic_undefined, rel);

	return 0;
}

static void relocate_section(Elf_Ehdr* elf, Elf_Shdr* shdr)
{
	Elf_Shdr* symtab = get_section(elf, shdr->sh_link);
	Elf_RelM* rel = get_section_data(elf, shdr);
	Elf_Sym* syms = get_section_data(elf, symtab);

	Elf_Shdr* section = get_section(elf, shdr->sh_info);

	int nbsym = shdr->sh_size / shdr->sh_entsize;

	for (int i = 0; i < nbsym; i++) {
		Elf_Sym* symbol = syms + ELF_R_SYM(rel[i].r_info);
		void* reloc_vaddr = get_vaddr(section) + rel[i].r_offset;
		void* reloc_laddr = get_laddr(section) + rel[i].r_offset;
		char* sym_name;
		char* addr;
		Elf_Addr value;

		switch (symbol->st_shndx) {
		case SHN_UNDEF:
			/*
			 * The symbol comes from the kernel or another
			 * module, so resolve it
			 */
			sym_name = get_name(elf, symtab->sh_link,
					    symbol->st_name);
			value = (Elf_Addr)symbol_lookup(sym_name);
			if (!value)
				undef_assign_value(reloc_vaddr, reloc_laddr,
						   sym_name, &(rel[i]));
			else
				assign_value(reloc_vaddr, reloc_laddr, value,
					     &(rel[i]));
			break;
		case SHN_COMMON:
			/*
			 * "Local" global symbol in a module should be
			 * in the .bss section.
			 */
			klog("Common symbol are not supported\n");
			break;
		case SHN_ABS:
			break;
		default:
			addr = get_laddr(get_section(elf, symbol->st_shndx));
			value = (Elf_Addr)(addr + symbol->st_value);
			assign_value(reloc_vaddr, reloc_laddr, value, &(rel[i]));
			break;
		}
	}
}

/*
 * Count the number of loadable section
 */
static int nb_loadable_section(Elf_Ehdr* elf)
{
	int res = 0;

	for (int i = 1; i < elf->e_shnum; i++) {
		Elf_Shdr* shdr = get_section(elf, i);
		if (shdr->sh_size
		    && (shdr->sh_type == SHT_PROGBITS
			|| shdr->sh_type == SHT_NOBITS))
			res++;
	}
	return res;
}

struct load_section {
	const char* name;
	struct reloc* (*load)(Elf_Ehdr* elf, Elf_Shdr* shdr);

	struct list_node next;
};

static struct list_node load_section_handlers = LIST_INIT(load_section_handlers);
static spinlock_t load_section_handlers_lock = SPINLOCK_INIT;

int add_section_loader(const char* name,
		       struct reloc* (*load)(Elf_Ehdr* elf, Elf_Shdr* shdr))
{
	struct load_section* handler = early_alloc(sizeof(struct load_section));
	if (!handler)
		return -ENOMEM;

	handler->name = name;
	handler->load = load;
	list_init(&handler->next);

	spinlock_lock(&load_section_handlers_lock);
	list_insert(&load_section_handlers, &handler->next);
	spinlock_unlock(&load_section_handlers_lock);

	return 0;
}
EXPORT_SYMBOL(add_section_loader);

/*
 * Load the section according to it's type
 */
struct reloc* default_load_section(Elf_Ehdr* elf, Elf_Shdr* shdr)
{
	void* vaddr = early_alloc(shdr->sh_size);

	if (shdr->sh_type == SHT_NOBITS)
		memset(vaddr, 0, shdr->sh_size);
	else
		memcpy(vaddr, get_section_data(elf, shdr), shdr->sh_size);

	struct reloc* res = early_alloc(sizeof(struct reloc));
	res->reloc_laddr = vaddr;
	res->reloc_vaddr = vaddr;

	return res;
}
EXPORT_SYMBOL(default_load_section);

static struct reloc* load_section(Elf_Ehdr* elf, Elf_Shdr* shdr)
{
	struct load_section* handler = NULL;
	spinlock_lock(&load_section_handlers_lock);
	lfor_each_entry(&load_section_handlers, handler, next) {
		if (!strcmp(section_name(elf, shdr->sh_name), handler->name)) {
			struct reloc* res = handler->load(elf, shdr);
			spinlock_unlock(&load_section_handlers_lock);
			return res;
		}
	}
	spinlock_unlock(&load_section_handlers_lock);

	klog("Unhandled section: %s\n", section_name(elf, shdr->sh_name));
	return NULL;
}

static struct modinfo* check_module(Elf_Ehdr* elf)
{
	if (!IS_ELF(*elf) || elf->e_machine != EM_MACHINE)
		return NULL;

	Elf_Shdr* modinfo = get_named_section(elf, ".modinfo");
	if (!modinfo)
		return NULL;

	/* Check if the module is compatible */
	struct modinfo* m = get_section_data(elf, modinfo);

	if (m->major != MAJOR || m->minor != MINOR)
		return NULL;

	return m;

}

struct mod_init_handler {
	int (*init)(struct module* mod);
	struct list_node next;
};

struct list_node pre_init_handlers = LIST_INIT(pre_init_handlers);
struct list_node post_init_handlers = LIST_INIT(post_init_handlers);
static spinlock_t init_handlers_lock = SPINLOCK_INIT;

static int register_init_module_handler(int (*handler)(struct module* mod),
					struct list_node* list)
{
	struct mod_init_handler* new_handler;
	new_handler = early_alloc(sizeof(struct mod_init_handler));
	if (!new_handler)
		return -ENOMEM;

	new_handler->init = handler;
	list_init(&new_handler->next);
	spinlock_lock(&init_handlers_lock);
	list_insert(list, &new_handler->next);
	spinlock_unlock(&init_handlers_lock);

	return 0;
}

int register_pre_init_module_handler(int (*handler)(struct module* mod))
{
	return register_init_module_handler(handler, &pre_init_handlers);
}
EXPORT_SYMBOL(register_pre_init_module_handler);

int register_post_init_module_handler(int (*handler)(struct module* mod))
{
	return register_init_module_handler(handler, &post_init_handlers);
}
EXPORT_SYMBOL(register_post_init_module_handler);

static int internal_mod_end_loading(struct module* mod)
{
	struct mod_init_handler* handler;
	spinlock_lock(&init_handlers_lock);
	lfor_each_entry(&pre_init_handlers, handler, next)
		handler->init(mod);
	spinlock_unlock(&init_handlers_lock);

	if (mod->info.init_once)
		mod->info.init_once();

	if (mod->info.init)
		mod->info.init();

	spinlock_lock(&init_handlers_lock);
	lfor_each_entry(&post_init_handlers, handler, next)
		handler->init(mod);
	spinlock_unlock(&init_handlers_lock);

	return 0;
}

static int add_deps(struct module* from, struct module* to)
{
	assert(from);
	assert(to);

	struct mod_dep* mdep_to = early_alloc(sizeof(struct mod_dep));
	if (!mdep_to)
		return -ENOMEM;

	mdep_to->mod = from;
	list_init(&mdep_to->next_dep);
	list_insert(to->dep_modules, &mdep_to->next_dep);

	struct mod_dep* mdep_from = early_alloc(sizeof(struct mod_dep));
	if (!mdep_from) {
		list_remove(&mdep_to->next_dep);
		early_free(mdep_to, sizeof(struct mod_dep));
		return -ENOMEM;
	}
	mdep_from->mod = to;
	list_init(&mdep_from->next_dep);
	list_insert(from->deps, &mdep_from->next_dep);

	return 0;
}

static struct module* handle_mod_deps(struct modinfo* mod, int* loaded,
				      struct modinfo* mod_start,
				      struct modinfo* mod_end,
				      int (*end_loading)(struct module* mod))
{
	loaded[mod - mod_start] = 1;

	struct module* res = early_alloc(sizeof(struct module));
	if (!res)
		return make_err_ptr(-ENOMEM);
	res->dep_modules = early_alloc(sizeof(struct list_node));
	if (!res->dep_modules)
		return make_err_ptr(-ENOMEM);
	list_init(res->dep_modules);

	res->deps = early_alloc(sizeof(struct list_node));
	if (!res->deps)
		return make_err_ptr(-ENOMEM);
	list_init(res->deps);

	u64 dep = mod->mod_deps;

	struct module* m;
	struct modinfo* minfo = mod_start;
	while (dep && minfo < mod_end) {
		if (dep & minfo->mod_type && !loaded[minfo - mod_start]) {

			dep &= ~(minfo->mod_type & dep);

			m = handle_mod_deps(minfo, loaded, mod_start,
					    mod_end, end_loading);
			if (is_err_ptr(m))
				return m;

			spinlock_lock(&modules_lock);
			int ret = add_deps(res, m);
			spinlock_unlock(&modules_lock);
			if (ret < 0)
				return make_err_ptr(ret);
		}

		minfo++;
	}

	/*
	 * Build the graph of modules for already loaded modules.
	 */
	spinlock_lock(&modules_lock);
	lfor_each_entry(&modules, m, mods) {
		if (m->info.mod_type & mod->mod_deps) {
			dep &= ~(m->info.mod_type & mod->mod_deps);

			int ret = add_deps(res, m);
			if (ret < 0) {
				spinlock_unlock(&modules_lock);
				return make_err_ptr(ret);
			}
		}
	}
	spinlock_unlock(&modules_lock);

	assert(dep == 0);

	memcpy(&res->info, mod, sizeof(struct modinfo));
	int ret = end_loading(res);
	if (ret < 0)
		return make_err_ptr(ret);

	list_init(&res->mods);
	spinlock_lock(&modules_lock);
	list_insert(modules.prev, &res->mods);
	spinlock_unlock(&modules_lock);

	return res;
}

extern struct modinfo _modinfo_start[];
extern struct modinfo _modinfo_end[];

static int init_internal_modules(void)
{
	struct modinfo* mod = _modinfo_start;
	struct modinfo* mod_end = _modinfo_end;

	int mod_nb = mod_end - mod;
	int* loaded_mods = early_alloc(mod_nb * sizeof(int));
	if (!loaded_mods)
		return -ENOMEM;

	memset(loaded_mods, 0, mod_nb * sizeof(int));

	int idx = 0;
	while (mod < mod_end) {
		if (!loaded_mods[idx++]) {
			struct module* m;
			m = handle_mod_deps(mod, loaded_mods, _modinfo_start,
					    mod_end, internal_mod_end_loading);
			if (is_err_ptr(m))
				return get_err_ptr(m);
		}
		mod++;
	}
	early_free(loaded_mods, mod_nb * sizeof(int));

	return 0;
}

/* elf is a valid module, with dependency loaded */
static int do_load_module(Elf_Ehdr* elf, struct module* mod)
{
	int nb = nb_loadable_section(elf);

	mod->nb_section = nb;
	mod->sections = early_alloc(nb * sizeof (struct section));
	if (!mod->sections)
		return -ENOMEM;

	nb = 0;
	for (int i = 1; i < elf->e_shnum; i++) {
		Elf_Shdr* shdr = get_section(elf, i);
		if (shdr->sh_size &&
		    (shdr->sh_type == SHT_PROGBITS
		    || shdr->sh_type == SHT_NOBITS)) {
			mod->sections[nb].reloc = load_section(elf, shdr);
			mod->sections[nb].len = shdr->sh_size;

			const char *name = section_name(elf, shdr->sh_name);
			size_t namelen = strlen(name);
			mod->sections[nb].name = early_alloc(namelen + 1);
			if (!mod->sections[nb].name)
				return -ENOMEM;
			strcpy(mod->sections[nb].name, name);
			mod->sections[nb].name[namelen] = '\0';

			/*
			 * Store the relocation information of the section
			 * directly into the section, so that it can be access
			 * when needed. The ->sh_addr is useless after loading
			 * the section, so use it!
			 */
			shdr->sh_addr = (Elf_Addr)mod->sections[nb].reloc;
			nb++;
		}
	}

	for (int i = 1; i < elf->e_shnum; i++) {
		Elf_Shdr* shdr = get_section(elf, i);
		if (shdr->sh_type == SHT_REL_MACHINE)
			relocate_section(elf, shdr);
	}

	Elf_Shdr* ksyms = get_named_section(elf, ".ksym");
	if (ksyms) {
		/*
		 * The loaded module has exported symbols, add them to
		 * the exported symbol list so that other modules can
		 * use them.
		 */
		struct ksym* sym = get_vaddr(ksyms);
		int nbsym = ksyms->sh_size / sizeof(struct ksym);

		for (int i = 0; i < nbsym; i++, sym++) {
			int ret = export_symbol(sym);
			if (ret < 0)
				return ret;
		}
	}

	Elf_Shdr* modinfo = get_named_section(elf, ".modinfo");
	struct modinfo* m = get_vaddr(modinfo);
	memcpy(&mod->info, m, sizeof(struct modinfo));

	return internal_mod_end_loading(mod);
}

int load_module(Elf_Ehdr* elf)
{
	return load_modules(&elf, 1);
}

static int end_elf_loading(struct module* mod)
{
	/* As seen in the load_modules function, mod->info.name actually
	 * temporarily contains a pointer to the elf itself */
	return do_load_module((Elf_Ehdr*)mod->info.name, mod);
}

int load_modules(Elf_Ehdr** modules, u32 mod_nb)
{
	if (!mod_nb)
		return 0;

	struct modinfo* mods = early_alloc(mod_nb * sizeof(struct modinfo));
	if (!mods)
		return -ENOMEM;

	int j = 0;
	int i = 0;
	while (modules[i]) {
		struct modinfo* m = check_module(modules[i]);
		if (m) {
			memcpy(&mods[j], m, sizeof(struct modinfo));
			mods[j].name = (void*)modules[i];
			j++;
		}
		i++;
	}

	int ret;
	int* loaded = early_alloc(j * sizeof(int));
	if (!loaded) {
		ret = -ENOMEM;
		goto out;
	}
	memset(loaded, 0, j * sizeof(int));

	struct modinfo* mod_end = &mods[j + 1];
	struct modinfo* mod = mods;
	i = 0;
	while (mod < mod_end) {
		if (!loaded[i++]) {
			struct module* m;
			m = handle_mod_deps(mod, loaded, mods, mod_end,
					    end_elf_loading);
			if (is_err_ptr(m)) {
				ret = get_err_ptr(m);
				goto out;
			}
		}
		mod++;
	}

	ret = 0;
out:
	if (loaded)
		early_free(loaded, j * sizeof(int));

	early_free(mods, mod_nb * sizeof(struct modinfo));

	return ret;
}

void init_module_loader(void)
{
	static const char* default_section_name[] = {
		".text", ".data", ".rodata", ".bss", ".ksym", ".modinfo",
		".comment", ".init", ".init_once", ".exit", ".exit_once"
	};
	for (unsigned int i = 0; i < array_size(default_section_name); i++)
		if (add_section_loader(default_section_name[i],
				       default_load_section) < 0)
			panic("Could not init the module loader.");

	export_kernel_syms();
	init_internal_modules();
}
