/*
 * File: loader.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: stage3 bootloader which is run after grub. Its only goal
 * is to modify grub's structure and then launch the kernel.
 */

#include <kernel/elf.h>
#include <kernel/string.h>
#include <kernel/stos.h>

#include "multiboot.h"
#include "pagination.h"

static void* boot_brk;

static void* boot_alloc(size_t size)
{
	void* res = boot_brk;
	boot_brk += size;
	return res;
}

static void translate_grub_mmap(multiboot_info_t* multiboot,
				struct boot_segments* segs)
{
	memory_map_t* current_mmap = (void*)multiboot->mmap_addr;
	memory_map_t* last_mmap = (void*)current_mmap + multiboot->mmap_length;
	int i = 0;
	while (current_mmap < last_mmap) {
		segs->segments[i].base_addr = current_mmap->base_addr_low;
		segs->segments[i].base_addr |= (u64)current_mmap->base_addr_high << 32;

		segs->segments[i].length = current_mmap->length_low;
		segs->segments[i].length |= (u64)current_mmap->length_high << 32;

		segs->segments[i].type = current_mmap->type;

		i++;

		unsigned long size = current_mmap->size + sizeof(unsigned long);
		current_mmap = (void*)current_mmap + size;
	}
	segs->seg_nb = i;
}

void loader(unsigned long magic, multiboot_info_t* multiboot)
{
	module_t* grub_mods = (void*)multiboot->mods_addr;
	size_t highest_kernel_addr = 0;
	struct boot_interface* boot;

	Elf32_Ehdr* hdr = (void*)grub_mods[0].mod_start;
	Elf32_Phdr* phdr = (void*)((char*)hdr + hdr->e_phoff);

	for (int i = 0; i < hdr->e_phnum; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		char* vaddr = PADDR(((char*)phdr[i].p_vaddr));
		size_t filesz = phdr[i].p_filesz;
		size_t memsz = phdr[i].p_memsz;
		size_t offset = phdr[i].p_offset;

		memcpy(vaddr, (void*)((char*)hdr + offset),
		       filesz);
		/*
		 * Sometimes, the size on memory is greater than the
		 * size on disk, zero the difference
		 */
		memset(vaddr + filesz, 0, memsz - filesz);

		if (highest_kernel_addr < phdr[i].p_paddr + memsz)
			highest_kernel_addr = phdr[i].p_paddr + memsz;
	}

	boot_brk = (void*)highest_kernel_addr;

	boot = boot_alloc(sizeof(struct boot_interface));
	boot->archdata = NULL;
	boot->mods = boot_alloc(sizeof(struct boot_modules));
	boot->mods->mod_nb = multiboot->mods_count;
	boot->mods->modules = boot_alloc(sizeof(Elf_Ehdr*) * boot->mods->mod_nb);

	size_t size = strlen((char*)multiboot->cmdline) + 1;
	boot->commandline = boot_alloc(size);
	memcpy(boot->commandline, (char*)multiboot->cmdline, size);
	boot->commandline = VADDR(boot->commandline);

	size = (void*)grub_mods[1].mod_end - (void*)grub_mods[1].mod_start;
	boot->initramfs = boot_alloc(size);
	memcpy(boot->initramfs, (void*)grub_mods[1].mod_start, size);
	boot->initramfs = VADDR(boot->initramfs);

	for (unsigned int i = 2; i < multiboot->mods_count; i++) {
		size_t mod_size = grub_mods[i].mod_end - grub_mods[i].mod_start;
		boot->mods->modules[i - 2] = boot_alloc(mod_size);
		memcpy(boot->mods->modules[i - 2],
		       (void*)grub_mods[i].mod_start,
		       mod_size);
		boot->mods->modules[i - 2] = VADDR(boot->mods->modules[i - 2]);
	}
	boot->mods->modules = VADDR(boot->mods->modules);
	boot->mods = VADDR(boot->mods);

	boot->segs = boot_alloc(sizeof(struct boot_segments));
	translate_grub_mmap(multiboot, boot->segs);

	boot->first_useable_addr = boot->segs->segments + boot->segs->seg_nb;
	boot->first_useable_addr = VADDR(boot->first_useable_addr);

	boot->segs = VADDR(boot->segs);

	launch_paginate_kernel((void*)hdr->e_entry, boot);
}
