/*
 * File: 16550.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement the PC16550D uart.
 *		http://www.national.com/ds/PC/PC16550D.pdf
 */

#include <kernel/arch/ioports.h>
#include <kernel/arch/types.h>
#include <kernel/char.h>
#include <kernel/errno.h>
#include <kernel/irq.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/raw_serial.h>
#include <kernel/sched.h>
#include <kernel/string.h>
#include <kernel/vfs.h>

/* Register addresses */
#define RBR_ADDR	0 /* Receiver Buffer, read access */
#define THR_ADDR	0 /* Transmitter Holding Buffer, write access */
#define IER_ADDR	1 /* Interrupt Enable */
#define IIR_ADDR	2 /* Interrupt Identification, read access */
#define FCR_ADDR	2 /* Fifo Control, write access */
#define LCR_ADDR	3 /* Line Control */
#define MCR_ADDR	4 /* Modem Control */
#define LSR_ADDR	5 /* Line Status */
#define MSR_ADDR	6 /* Modem Status */
#define SCR_ADDR	7 /* Scratch */

#define DLL_ADDR	0 /* Divisor Latch least significant byte */
#define DLM_ADDR	1 /* Divisor Latch most significant byte */

struct serial_dev {
	struct list_node next;
	struct char_dev* dev;
	int idx;

	u16 port;
	u8 irq;
	spinlock_t lock;
};

struct com_private {
	u8* buffer;
	unsigned int buffer_len;
	unsigned int position;

	struct sleep_queue q;

	struct serial_dev* dev;
};

static inline void set_dlab(int port)
{
	outb(port + LCR_ADDR, inb(port + LCR_ADDR) | (1 << 7));
}

static inline void unset_dlab(int port)
{
	outb(port + LCR_ADDR, inb(port + LCR_ADDR) & ~(1 << 7));
}

static void set_rate(int port, int rate)
{
	/*
	 * The UART has an internal clock of 1.8432MHz, which is then
	 * divided by 16, so the maximum frequency is 115,2Khz
	 */
	u16 divisor = 115200 / rate;
	/* A divisor of 0 is _not_ recommended */
	assert(divisor);

	set_dlab(port);
	outb(port + DLL_ADDR, divisor & 0xFF);
	outb(port + DLM_ADDR, divisor >> 8);
	unset_dlab(port);
}

static void configure_serial_port_nonlocked(int port, int rate)
{
	set_rate(port, rate);
	/* 8bits characters, no parity checks and disable dlab */
	outb(port + LCR_ADDR, 0x03);
}

#if 0
void configure_serial_port(int port_nb, int rate)
{
	u32 flags;
	spinlock_lock_irq(&com_locks[port_nb], &flags);
	configure_serial_port_nonlocked(port_nb, rate);
	spinlock_unlock_irq(&com_locks[port_nb], flags);
}
EXPORT_SYMBOL(configure_serial_port);
#endif

static int serial_getchar_nonlocked(int port)
{
	u8 lsr = inb(port + LSR_ADDR);

	if (!(lsr & 0x01))
		return -1;

	u8 chr = inb(port + RBR_ADDR);

	return chr;
}

#if 0
int serial_getchar(int port_nb)
{
	u32 flags;
	spinlock_lock_irq(&com_locks[port_nb], &flags);
	int ret = serial_getchar_nonlocked(port_nb);
	spinlock_unlock_irq(&com_locks[port_nb], flags);
	return ret;
}
EXPORT_SYMBOL(serial_getchar);
#endif

static void serial_putchar_nonlocked(u16 port, char c)
{
	while (!(inb(port + LSR_ADDR) & 0x20))
		continue;
	outb(port + THR_ADDR, c);
}

#if 0
void serial_putchar(int port_nb, char c)
{
	u32 flags;
	spinlock_lock_irq(&com_locks[port_nb], &flags);
	serial_putchar_nonlocked(port_nb, c);
	spinlock_unlock_irq(&com_locks[port_nb], flags);
}
EXPORT_SYMBOL(serial_putchar);
#endif

static int uart_handler(void* data)
{
	struct com_private* priv = data;

	u16 port = priv->dev->port;

	/* Does the uart sent the IRQ? */
	if (inb(port + IIR_ADDR) & 0x01)
		return 0;


	u32 flags;
	spinlock_lock_irq(&priv->dev->lock, &flags);

	char c;
	while ((c = serial_getchar_nonlocked(priv->dev->port)) != -1)
		priv->buffer[priv->position++] = c;

	spinlock_unlock_irq(&priv->dev->lock, flags);

	wake_up(&priv->q);

	return 1;
}

static int uart_open(struct inode* ino, struct file* f)
{
	struct serial_dev* dev = ino->i_fs_data;

	struct com_private* priv = kmalloc(sizeof(struct com_private));
	if (!priv)
		return -ENOMEM;

	priv->buffer = kmalloc(sizeof(u8) * 64);
	if (!priv->buffer) {
		kfree(priv, sizeof(struct com_private));
		return -ENOMEM;
	}
	priv->buffer_len = 64;
	priv->position = 0;
	priv->dev = dev;

	f->f_fs_data = priv;

	init_sleep_queue(&priv->q);

	configure_serial_port_nonlocked(dev->port, 9600);

	add_irq_handler(uart_handler, priv, dev->irq, IRQ_SHARED);

	unset_dlab(dev->port);
	/* Enable interrupts only for received data */
	outb(dev->port + IER_ADDR, 1);

	return 0;
}

static ssize_t uart_read(struct file* f, char* buf, size_t len)
{
	struct com_private* priv = f->f_fs_data;

	sleep_on(&priv->q, priv->position != 0);

	u32 flags;
	spinlock_lock_irq(&priv->dev->lock, &flags);
	size_t to_read = len > priv->position ? priv->position : len;
	memcpy(buf, priv->buffer, to_read);
	memcpy(priv->buffer, priv->buffer + to_read,
	       priv->position - to_read);
	priv->position -= to_read;
	spinlock_unlock_irq(&priv->dev->lock, flags);

	return to_read;
}

static ssize_t uart_write(struct file* f, const char* buf, size_t len)
{
	struct com_private* priv = f->f_fs_data;

	u32 flags;
	spinlock_lock_irq(&priv->dev->lock, &flags);
	for (size_t i = 0; i < len; i++)
		serial_putchar_nonlocked(priv->dev->port, buf[i]);
	spinlock_unlock_irq(&priv->dev->lock, flags);
	return len;
}

static void uart_release(struct refcnt* ref)
{
	struct file* f;
	f = container_of(ref, struct file, f_refcnt);

	kfree(f->f_fs_data, sizeof(struct com_private));
	kfree(f, sizeof(struct file));
}

static const struct file_ops uart_fops = {
	.open = uart_open,
	.read = uart_read,
	.write = uart_write,
	.release = uart_release
};

static int uart_chardev_add(struct char_dev* chr, u16 port, u8 irq)
{
	struct serial_dev* dev = kmalloc(sizeof(struct serial_dev));
	if (!dev)
		return -1;

	dev->dev = chr;
	dev->port = port;
	dev->irq = irq;
	dev->lock = SPINLOCK_INIT;
	dev->idx = register_minor(chr->dev, dev);

	return 0;
}

struct char_dev* uart_char_dev;

int register_serial_device(u16 port, u8 irq)
{
	return uart_chardev_add(uart_char_dev, port, irq);
}
EXPORT_SYMBOL(register_serial_device);

static void __init_once init(void)
{
	uart_char_dev = kmalloc(sizeof(struct char_dev));
	if (!uart_char_dev)
		return;

	uart_char_dev->ops = &uart_fops;
	register_char_dev(uart_char_dev, "com");
}

MODINFO {
	module_name("16550"),
	module_init_once(init),
	module_type(M_SERIAL),
	module_deps(M_VFS | M_CHAR | M_KMALLOC | M_IRQ | M_SCHED)
};
