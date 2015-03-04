/*
 * File: pvpanic.h
 * Author: Louis Feuvrier <manny@lse.epita.fr>
 */

#ifndef PVPANIC_H_
# define PVPANIC_H_

# include <kernel/i386/ioports.h>

# define PVPANIC_IOPORT (0x505)
# define PVPANIC_SET (1 << 0)

static inline void set_qemu_panicked_state(void)
{
	outb(PVPANIC_IOPORT, PVPANIC_SET);
}

#endif /* !PVPANIC_H_ */
