/*
 * File: elf.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implements I386 ELF function.
 *		Docs are Section 4-4:
 *		http://www.sco.com/developers/devspecs/abi386-4.pdf
 *
 */


#include <kernel/klog.h>
#include <kernel/elf.h>

void assign_value(Elf_Word* vaddr, Elf_Word* laddr, Elf_Addr value, Elf_RelM* rel)
{
	switch (ELF_R_TYPE(rel->r_info)) {
	case R_386_32:
		*vaddr += value;
		break;
	case R_386_PC32:
		*vaddr += value - (Elf_Word)laddr;
		break;
	default:
		klog("Unsuported relocation: %d\n", ELF_R_TYPE(rel->r_info));
		break;
	}
}
