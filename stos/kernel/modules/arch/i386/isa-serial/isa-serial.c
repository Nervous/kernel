/*
 * File: 16550_uart.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement the PC16550D uart.
 *		http://www.national.com/ds/PC/PC16550D.pdf
 */

#include <kernel/module.h>
#include <kernel/serial.h>

static const struct {
	u16 port;
	u8 irq;
} com_ports[] = {
	{ .port = 0x3f8, .irq = 4 },
	{ .port = 0x2f8, .irq = 3 },
	{ .port = 0x3e8, .irq = 4 },
	{ .port = 0x2e8, .irq = 3 },
};

static void __init_once init(void)
{
	for (size_t com = 0; com < sizeof(com_ports) / sizeof(*com_ports); ++com)
		register_serial_device(com_ports[com].port, com_ports[com].irq);
}

MODINFO {
	module_name("isa-serial"),
	module_init_once(init),
	module_deps(M_VFS | M_CHAR | M_KMALLOC | M_IRQ | M_SCHED | M_SERIAL)
};
