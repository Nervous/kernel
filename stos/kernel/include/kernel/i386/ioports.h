#ifndef I386_IOPORTS_H_
# define I386_IOPORTS_H_

# include <kernel/types.h>

static inline void outb(u16 port, u8 val)
{
	__asm__ volatile("outb %0, %1\n\t"
			 : /* No output */
			 : "a" (val), "d" (port));
}

static inline u8 inb(u16 port)
{
	u8 res;

	__asm__ volatile("inb %1, %0\n\t"
			 : "=&a" (res)
			 : "d" (port));
	return res;
}

static inline void outw(u16 port, u16 val)
{
	__asm__ volatile("outw %0, %1\n\t"
			 : /* No output */
			 : "a" (val), "d" (port));
}

static inline u16 inw(u16 port)
{
	u16 res;

	__asm__ volatile("inw %1, %0\n\t"
			 : "=&a" (res)
			 : "d" (port));
	return res;
}

static inline void outl(u16 port, u32 val)
{
	__asm__ volatile("outl %0, %1\n\t"
			 : /* No output */
			 : "a" (val), "d" (port));
}

static inline u32 inl(u16 port)
{
	u32 res;

	__asm__ volatile("inl %1, %0\n\t"
			 : "=&a" (res)
			 : "d" (port));
	return res;
}

#endif /* !IOPORTS_H_ */
