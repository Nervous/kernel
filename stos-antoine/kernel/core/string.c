/*
 * File: string.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#include <kernel/module.h>
#include <kernel/string.h>

size_t strlen(const char* s)
{
	const char* p = s;

	for (; *p; ++p)
		continue;
	return p - s;
}
EXPORT_SYMBOL(strlen);

size_t strnlen(const char* s, size_t n)
{
	const char* p = s;

	for (; *p && n != 0; ++p, n--)
		continue;
	return p - s;
}
EXPORT_SYMBOL(strnlen);

char* strcpy(char* dest, const char* src)
{
	char* res = dest;

	for (; (*dest++ = *src++); )
		continue;
	return res;
}
EXPORT_SYMBOL(strcpy);

char* strncpy(char* dest, const char* src, size_t n)
{
	size_t i;
	for (i = 0; i < n && src[i] != '\0'; i++)
		dest[i] = src[i];
	for ( ; i < n; i++)
		dest[i] = '\0';
	return dest;
}
EXPORT_SYMBOL(strncpy);

int strcmp(const char* s1, const char* s2)
{
	while (*s1 && *s2) {
		if (*s1 != *s2)
			return (unsigned char)*s1 - (unsigned char)*s2;
		s1++;
		s2++;
	}
	return (unsigned char)*s1 - (unsigned char)*s2;
}
EXPORT_SYMBOL(strcmp);

int strncmp(const char* s1, const char* s2, size_t len)
{
	while (--len && *s1 && *s2) {
		if (*s1 != *s2)
			return (unsigned char)*s1 - (unsigned char)*s2;
		s1++;
		s2++;
	}
	return (unsigned char)*s1 - (unsigned char)*s2;
}
EXPORT_SYMBOL(strncmp);

void* memset(void* s, int c, size_t n)
{
	char* data = s;

	while (n--)
		*data++ = c;

	return s;
}
EXPORT_SYMBOL(memset);

void* memcpy(void* dest, const void* src, size_t n)
{
	void* res = dest;

	while (n--)
		*((char*)dest++) = *((char*)src++);
	return res;
}
EXPORT_SYMBOL(memcpy);

int memcmp(const void* s1, const void* s2, size_t n)
{
	const unsigned char* cs1 = s1;
	const unsigned char* cs2 = s2;

	while (n--) {
		if (*cs1 != *cs2)
			return *cs1 - *cs2;
		cs1++;
		cs2++;
	}
	return 0;
}
EXPORT_SYMBOL(memcmp);
