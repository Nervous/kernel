/*
 * File: klog.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#include <kernel/arch/spinlock.h>
#include <kernel/klog.h>
#include <kernel/list.h>
#include <kernel/module.h>
#include <kernel/stos.h>

#define BUFLEN          1024

static struct list_node cons_list = LIST_INIT(cons_list);
static spinlock_t cons_lock = SPINLOCK_INIT;

void add_console(struct console_ops* ops)
{
	spinlock_lock(&cons_lock);
	list_insert(&cons_list, &ops->next_cons);
	spinlock_unlock(&cons_lock);
}
EXPORT_SYMBOL(add_console);

static char buf[BUFLEN];
static size_t buflen = 0;

static inline void putchar(char c)
{
	buf[buflen++] = c;

	if (buflen == BUFLEN - 1 || c == '\n') {
		spinlock_lock(&cons_lock);
		struct console_ops* cons;
		lfor_each_entry(&cons_list, cons, next_cons)
			cons->write(buf, buflen);
		spinlock_unlock(&cons_lock);
		buflen = 0;
	}
}

static void print_unsigned(size_t n)
{
	if (n >= 10) {
		print_unsigned(n / 10);
		n %= 10;
	}
	putchar(n + '0');
}

static void print_int(ssize_t n)
{
	if (n < 0) {
		putchar('-');
		n = -n;
	}
	print_unsigned(n);
}

static void print_hexa(size_t n)
{
	if (n >= 16) {
		print_hexa(n / 16);
		n %= 16;
	}
	if (n < 10)
		putchar(n + '0');
	else
		putchar((n  - 10) + 'A');
}

static void print_octal(size_t n)
{
	if (n >= 8) {
		print_octal(n / 8);
		n %= 8;
	}
	putchar(n + '0');
}

static void print_string(const char* s)
{
	while (*s)
		putchar(*s++);
}

static spinlock_t klog_lock = SPINLOCK_INIT;

void klog(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	spinlock_lock(&klog_lock);
	while (*fmt) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
			case 'd':
			case 'i':
				print_int(va_arg(ap, ssize_t));
				break;
			case 'X':
				print_hexa(va_arg(ap, size_t));
				break;
			case 'o':
				print_octal(va_arg(ap, size_t));
				break;
			case 'u':
				print_unsigned(va_arg(ap, size_t));
				break;
			case 's':
				print_string(va_arg(ap, const char *));
				break;
			case 'c':
				putchar(va_arg(ap, size_t));
				break;
			case '%':
				putchar('%');
				break;
			default:
				print_string("Unsupported format %");
				putchar(*fmt);
				putchar('\n');
				break;
			}
		} else
			putchar(*fmt);
		fmt++;
	}
	spinlock_unlock(&klog_lock);

	va_end(ap);
}
EXPORT_SYMBOL(klog);
