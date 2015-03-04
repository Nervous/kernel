/*
 * File: rwspinlock.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Readers-writer spinlock.
 */
#ifndef I386_RWSPINLOCK_H_
# define I386_RWSPINLOCK_H_

/**
 * A readers-writer spinlock.
 *
 * The lock is taken for writing if the value is < 0, taken for reading
 * if the value is > 0, and unlocked if the value is 0
 */
typedef long rw_spinlock_t;

# define RW_SPINLOCK_INIT	0

/**
 * Initialize the spinlock.
 * @lock: The lock.
 */
static inline void rw_spinlock_init(rw_spinlock_t* lock)
{
	*lock = RW_SPINLOCK_INIT;
}

/**
 * Lock the lock for reading. Multiple thread can lock for reading at
 * the same time.
 * @lock: The lock.
 */
static inline void rw_spinlock_rdlock(rw_spinlock_t* lock)
{
	rw_spinlock_t old_val;
	rw_spinlock_t new_val;

	__asm__ volatile("1:\n\t"
			 "movl %0, %1\n\t"
			 "testl %1, %1\n\t"
			 "js 1b\n\t"
			 "movl %1, %2\n\t"
			 "incl %2\n\t"
			 "lock cmpxchgl %2, %0\n\t"
			 "jnz 1b\n\t"
			 : "=m" (*lock), "=&a" (old_val), "=&r" (new_val));
}

/**
 * Unlock the previously taken for reading lock.
 * @lock: The lock.
 */
static inline void rw_spinlock_rdunlock(rw_spinlock_t* lock)
{
	__asm__ volatile("lock decl %0\n\t"
			 : "=m" (*lock));
}

/**
 * Lock the lock for writing. Only one thread can take the lock for
 * writing.
 * @lock: The lock.
 */
static inline void rw_spinlock_wrlock(rw_spinlock_t* lock)
{
	rw_spinlock_t old_val;
	rw_spinlock_t new_val;

	__asm__ volatile("1:\n\t"
			 "movl %0, %1\n\t"
			 "testl %1, %1\n\t"
			 "jnz 1b\n\t"
			 "movl %1, %2\n\t"
			 "decl %2\n\t"
			 "lock cmpxchgl %2, %0\n\t"
			 "jnz 1b\n\t"
			 : "=m" (*lock), "=&a" (old_val), "=&r" (new_val));
}

/**
 * Unlock the previously taken for writing lock.
 * @lock: The lock.
 */
static inline void rw_spinlock_wrunlock(rw_spinlock_t* lock)
{
	__asm__ volatile("lock incl %0\n\t"
			 : "=m" (*lock));
}


#endif /* !I386_RWSPINLOCK_H_ */
