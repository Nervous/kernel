/*
 * File: cpuvar.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Manage the local cpu variables.
 */

#include <kernel/arch/cpuvar.h>
#include <kernel/arch/crx.h>
#include <kernel/arch/pm.h>
#include <kernel/frame.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/module_loader.h>
#include <kernel/page.h>
#include <kernel/string.h>
#include <kernel/stos.h>

/*
 * Virtual address of the BSP cpuvar zone
 */
static void* bsp_cpuvar_addr;

/*
 * Size of the cpuvar zone.
 */
static atomic_t cpuvar_size = 0;

static struct reloc* load_cpuvar_section(Elf_Ehdr* elf, Elf_Shdr* shdr)
{
	/* FIXME: Does it make sense to have SHT_PROGBITS for cpuvar ? */
	if (shdr->sh_type != SHT_NOBITS)
		panic("cpuvar relocation should be SHT_NOBITS");

	struct reloc* res = kmalloc(sizeof(struct reloc));
	if (!res)
		return NULL;

	atomic_t old_cpuvar_size = atomic_add(&cpuvar_size, shdr->sh_size);

	res->reloc_vaddr = bsp_cpuvar_addr + old_cpuvar_size;
	res->reloc_laddr = (void*)old_cpuvar_size;

	memset(res->reloc_vaddr, 0, shdr->sh_size);

	return res;
}

static void __init_once init_cpuvar_section(void)
{
	add_section_loader(".cpuvar", load_cpuvar_section);
}

static void __init init_cpuvar(void)
{
	struct frame* pdbr = phys_to_frame(get_cr3());

	bsp_cpuvar_addr = alloc_pages(pdbr, 1);
	if (!bsp_cpuvar_addr)
		goto err;

	struct frame* f = alloc_frame();
	if (!f)
		goto err;

	map_pages(pdbr, bsp_cpuvar_addr, &f, 1, P_KERNEL);

	set_gdt_gate((u32)bsp_cpuvar_addr, 1, DATA_SEGMENT(0),
		     3, KERNEL_FS);
	set_fs(KERNEL_FS_SEL);

	return;

err:
	panic("Could not initialize cpuvar");
}

/* FIXME: kmalloc may use the cpuvar module. */
MODINFO {
	module_name("cpuvar"),
	module_init_once(init_cpuvar_section),
	module_init(init_cpuvar),
	module_type(M_CPUVAR),
	module_deps(M_MEMORY_INIT | M_PAGINATION | M_KMALLOC | M_FRAME_ALLOCATOR)
};
