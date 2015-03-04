#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/stos.h>
#include <kernel/string.h>

#define META '%'

static unsigned char itoa(char* buff, unsigned int n, unsigned char base)
{
	unsigned char i = 0;

	if (n == 0)
		buff[i++] = '0';
	for (; n != 0; i += 1, n /= base)
		buff[i] = "0123456789abcdef"[(n % base)];
	buff[i--] = '\0';

	size_t len = i + 1;

	for (unsigned char j = 0; j < i; j += 1, i -= 1) {
		char tmp = buff[i];
		buff[i] = buff[j];
		buff[j] = tmp;
	}

	return len;
}

static size_t print_unsigned(char* buf, unsigned int input, char base)
{
	return itoa(buf, input, base);
}

static size_t print_int(char* buf, int input, char base)
{
	size_t len = 0;
	if (input < 0) {
		*buf++ = '-';
		input = -input;
		len += 1;
	}

	return len + print_unsigned(buf, input, base);
}

static size_t print_str(char* buf, char* input)
{
	if (input == NULL) {
		strcpy(buf, "(nil)");
		return 5;
	}

	strcpy(buf, input);
	return (size_t)strlen(input);
}

size_t snprintf(char* buf, size_t size, const char* format, ...)
{
	size_t len = 0;
	va_list ap;

	va_start(ap, format);

	for (u32 i = 0; format[i] != '\0' && len < size;
		i += (format[i] == META ? 2 : 1)) {
		if (format[i] != META)
			buf[len++] = format[i];
		else if (format[i + 1] == META)
			buf[len++] = format[i++];
		else {
			if (format[i + 1] == 'x' || format[i + 1] == 'p')
				len += print_unsigned(buf + len,
					va_arg(ap, int), 0x10);
			else if (format[i + 1] == 'o')
				len += print_unsigned(buf + len,
					va_arg(ap, int), 010);
			else if (format[i + 1] == 'u')
				len += print_unsigned(buf + len,
					va_arg(ap, int), 10);
			else if (format[i + 1] == 'd')
				len += print_int(buf + len,
					va_arg(ap, int), 10);
			else if (format[i + 1] == 's')
				len += print_str(buf + len, va_arg(ap, char*));
			else if (format[i + 1] == 'c')
				buf[len++] = (char)va_arg(ap, int);
			else
				len += 1;
		}
	}

	buf[len] = '\0';
	return len;
}
EXPORT_SYMBOL(snprintf);

/*
 * This function is copied from the bionic library and should not be modified
 */
char* strtok_r(char* str, const char* delim, char** last)
{
	char* spanp;
	int c, sc;
	char* tok;

	if (str == NULL && (str = *last) == NULL)
		return NULL;

cont:
	c = *str++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {
		*last = NULL;
		return NULL;
	}
	tok = str - 1;

	while (1) {
		c = *str++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					str = NULL;
				else
					str[-1] = '\0';
				*last = str;
				return (tok);
			}
		} while (sc != 0);
	}
}
EXPORT_SYMBOL(strtok_r);

char* strdup(const char* s)
{
	size_t len = strlen(s) + 1;
	char* new = kmalloc(len);
	if (new == NULL)
		return make_err_ptr(-ENOMEM);
	return strncpy(new, s, len);
}
EXPORT_SYMBOL(strdup);

char* strchr(const char* s, int c)
{
	for (; *s; s++)
		if (*s == c)
			return (char*)s;
	return NULL;
}
EXPORT_SYMBOL(strchr);
