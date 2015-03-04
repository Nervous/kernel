/*
 * File: elf.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#ifndef I386_ELF_H_
# define I386_ELF_H_

# define ELFSIZE 32
# define EM_MACHINE EM_386

# define R_386_NONE	0		/* No reloc */
# define R_386_32	1		/* Direct 32 bit  */
# define R_386_PC32	2		/* PC relative 32 bit */
# define R_386_GOT32	3		/* 32 bit GOT entry */
# define R_386_PLT32	4		/* 32 bit PLT address */
# define R_386_COPY	5		/* Copy symbol at runtime */
# define R_386_GLOB_DAT	6		/* Create GOT entry */
# define R_386_JMP_SLOT	7		/* Create PLT entry */
# define R_386_RELATIVE	8		/* Adjust by program base */
# define R_386_GOTOFF	9		/* 32 bit offset to GOT */
# define R_386_GOTPC	10		/* 32 bit PC relative offset to GOT */
/* Keep this the last entry.  */
# define R_386_NUM	11

/*
 * I386 only support SHT_REL
 */
# define SHT_REL_MACHINE SHT_REL
# define Elf_RelM Elf_Rel

#endif /* !I386_ELF_H_ */
