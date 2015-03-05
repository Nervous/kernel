/*
 * File: minilibc.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 * Description: Mini libc for the bootlodaer.
 */

#include <kernel/string.h>

void* memset(void* s, int c, size_t n)
{
	char* tmp = s;

	while (n--)
		*tmp++ = c;
	return s;
}

void* memcpy(void* dest, const void* src, size_t n)
{
	void* res = dest;

	while (n--)
		*((char*)dest++) = *((char*)src++);
	return res;
}

size_t strlen(const char* s)
{
	const char* p = s;

	for (; *p; ++p)
		continue;
	return p - s;
}
