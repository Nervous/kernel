/*
 * File: pm.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Function relative to the segmentation
 */

#ifndef I386_PM_H_
# define I386_PM_H_

# define SELECTOR(Entry, Rpl)	((Entry << 3) + Rpl)

# define KERNEL_CS		1
# define KERNEL_CS_SEL		SELECTOR(KERNEL_CS, 0)
# define KERNEL_DS		2
# define KERNEL_DS_SEL		SELECTOR(KERNEL_DS, 0)
# define KERNEL_FS		3
# define KERNEL_FS_SEL		SELECTOR(KERNEL_FS, 0)

# define USER_CS		4
# define USER_CS_SEL		SELECTOR(USER_CS, 3)
# define USER_DS		5
# define USER_DS_SEL		SELECTOR(USER_DS, 3)
# define USER_FS		6
# define USER_FS_SEL		SELECTOR(USER_FS, 3)

# define KERNEL_TSS		7
# define KERNEL_TSS_SEL		SELECTOR(KERNEL_TSS, 0)

# define KERNEL_TSS_DF		8
# define KERNEL_TSS_DF_SEL	SELECTOR(KERNEL_TSS_DF, 0)

# ifndef __ASSEMBLY__

#  include <kernel/types.h>

#define CODE_SEGMENT(Dpl) (((Dpl) << 5) | (1 << 4) | 0xA)
#define DATA_SEGMENT(Dpl) (((Dpl) << 5) | (1 << 4) | 0x3)
#define TSS_SEGMENT(Dpl) (((Dpl) << 5) | 0x9)

/* Return 0 if the gate cannot be added, 1 otherwise. */
int set_gdt_gate(u32 base, u32 limit, u8 access, u8 granularity, int n);

#  define SEG_REG(Name)					\
	static inline void set_ ## Name (u16 v)		\
	{						\
		__asm__("movw %0, %%" #Name "\n\t"	\
			: /* No output */		\
			: "r" (v)			\
			: "memory");			\
	}						\
	static inline u16 get_ ## Name (void)		\
	{						\
		u16 ret;				\
							\
		__asm__("movw %%" #Name ", %0\n\t"	\
			: "=&r" (ret));			\
		return ret;				\
	}

SEG_REG(ds)
SEG_REG(es)
SEG_REG(fs)
SEG_REG(gs)
SEG_REG(ss)

static inline void set_cs(u32 v)
{
	__asm__("pushl %0\n\t"
		"pushl $1f\n\t"
		"lret\n\t" /* pop EIP, then %cs */
		"1:\n\t"
		: /* No output */
		: "r" (v)
		: "memory");
}

static inline u16 get_cs(void)
{
	u16 ret;

	__asm__("movw %%cs, %0\n\t"
		: "=&r" (ret));
	return ret;
}

# endif /* !__ASSEMBLY__ */

#endif /* !I386_PM_H_ */
