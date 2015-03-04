/*
 * File: string.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#ifndef STRING_H_
# define STRING_H_

# include <kernel/types.h>

size_t strlen(const char* s);
size_t strnlen(const char* s, size_t n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t len);
void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

#endif /* !STRING_H_ */
