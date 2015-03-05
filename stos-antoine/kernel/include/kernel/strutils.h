#ifndef KSTR_UTILS_H_
# define KSTR_UTILS_H_

# include <kernel/types.h>

size_t snprintf(char* buf, size_t size, const char* format, ...);
char* strtok_r(char* str, const char* delim, char** last);
char* strdup(const char* s);
char* strchr(const char* s, int c);

#endif /* !KSTR_UTILS_H_ */
