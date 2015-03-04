/*
 * File: display.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: i386 write.
 */

#include <kernel/string.h>
#include <kernel/klog.h>
#include <kernel/i386/ioports.h>

#include "display.h"

//static char* fb = (char*)0xb8000;
static char* fb = (char*)0xC00B8000;
static unsigned int x_pos;
static unsigned int y_pos;

static void clear_screen(void)
{
	for (int i = 0; i < 25 * 80 * 2; i++) {
		fb[i++] = ' ';
		fb[i] = 0x7;
	}
}

static void scroll(void)
{
	memcpy(fb, fb + (80 * 2), 80 * 24 * 2);

	for (int i = 0; i < 80 * 2; i++) {
		fb[80 * 24 * 2 + i++] = ' ';
		fb[80 * 24 * 2 + i] = 0x7;
	}
}

/* Update the cursor's position. */
static void cursor_update(void)
{
	x_pos++;
	if (x_pos >= 80) {
		x_pos = 0;
		if (++y_pos == 25) {
			x_pos = 0;
			y_pos--;
			scroll();
		}
	}

	unsigned int pos = y_pos * 80 + x_pos;

	outb(0x3D4, 14);
	outb(0x3D5, (pos >> 8));
	outb(0x3D4, 15);
	outb(0x3D5, (pos & 0xFF));
}

static int i386_write(char* buf, size_t len)
{
	int res = len;

	while (len--) {
		if (*buf == '\n') {
			x_pos = 80;
			cursor_update();
			buf++;
			continue;
		}
		fb[(80 * y_pos + x_pos) * 2] = *buf++;
		fb[(80 * y_pos + x_pos) * 2 + 1] = 0x7;
		cursor_update();
	}

	return res;
}

static struct console_ops i386_screen = {
	.name = "i386",
	.write = i386_write
};

#ifdef EARLY_KLOG_SERIAL
static int i386_serial_write(char *buf, size_t len) {
	int res = len;

	while (len--) {
		outb(EARLY_KLOG_SERIAL_PORT, *buf++);
	}

	return res;
}

static struct console_ops i386_serial = {
	.name = "serial",
	.write = i386_serial_write
};
#endif

void init_display(void)
{
	x_pos = 0;
	y_pos = 0;

	clear_screen();
	add_console(&i386_screen);
#ifdef EARLY_KLOG_SERIAL
	add_console(&i386_serial);
#endif
}
