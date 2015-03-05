#ifndef MODULES_H_
# define MODULES_H_

# include <kernel/elf.h>
# include <kernel/types.h>

struct boot_modules {
	u32 mod_nb;
	Elf_Ehdr** modules;
};

#endif /* !MODULES_H_ */
