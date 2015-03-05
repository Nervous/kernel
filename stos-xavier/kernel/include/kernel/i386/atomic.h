#ifndef I386_ATOMIC_H_
# define I386_ATOMIC_H_

# include <kernel/types.h>

typedef uword atomic_t;

/*
 * Return the value of the atomic variable
 */
static inline atomic_t atomic_add(atomic_t* n, int v)
{
	atomic_t ret = v;
	__asm__ volatile("lock xaddl %1, %0\n\t"
			 : "=m" (*n), "+r" (ret));
	return ret + v;
}

/*
 * Return the value of the atomic variable
 */
static inline atomic_t atomic_inc(atomic_t* n)
{
	return atomic_add(n, 1);
}

/*
 * Return the value of the atomic variable
 */
static inline atomic_t atomic_dec(atomic_t* n)
{
	return atomic_add(n, -1);
}

static inline void atomic_set(atomic_t* n, int v)
{
	__asm__ volatile("movl %1, %0\n\t"
			 : "=m" (*n)
			 : "r" (v));
}

#endif /* !I386_ATOMIC_H_ */
