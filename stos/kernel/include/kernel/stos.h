/*
 * File: stos.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Main kernel header, define some useful macros.
 */

#ifndef STOS_H_
# define STOS_H_

# include <kernel/panic.h>
# include <kernel/types.h>

# define NULL ((void*)0)

# define JUNK_PTR ((void*)0xD0D0D0D0)

# define offsetof(Type, Field)          \
    ((size_t)&(((Type*)0)->Field))

# define container_of(Ptr, Type, Field) ({			\
	const __typeof__(((Type*)0)->Field)* __ptr = (Ptr);	\
        (Type*)((char*)__ptr - offsetof(Type, Field)); })

# define __attribute(...)	__attribute__((__VA_ARGS__))

# define __section(Name)	__attribute(section(Name))
# define __unused		__attribute(unused)
# define __used			__attribute(used)
# define __align(N)		__attribute(aligned(N))
# define __packed		__attribute(packed)
# define __forceinline		inline __attribute(always_inline)
# define __syscall		__attribute(regparm(0))
# define __cpuvar		__section(".cpuvar,\"aw\",@nobits #")

/*
 * Assert
 */
#define TOSTRING_(X) #X
#define TOSTRING(X) TOSTRING_(X)

# define assert(Expr)				\
	(Expr) ? (void)0 : panic("Assert : " __FILE__ ":" \
				 TOSTRING(__LINE__) ": " TOSTRING(Expr))

/*
 * Variadic macros.
 */
typedef __builtin_va_list va_list;

# define va_start(ap, param)	__builtin_va_start(ap, param)
# define va_end(ap)		__builtin_va_end(ap)
# define va_arg(ap, type)	__builtin_va_arg(ap, type)

/*
 * Alignement macros.
 */

/* align to the next power of two. (No change if already aligned) */
# define align2_up(v, d)	((((v) - 1) & ~((d) - 1)) + (d))

/* align to the previous power of two. (No change if already aligned) */
# define align2_down(v, d)	((v) & ~((d) - 1))

# define align_up(v, d)		((((v) + (d) - 1) / (d)) * (d))

# define array_size(array)	(sizeof(array) / sizeof(*array))

#endif /* !STOS_H_ */
