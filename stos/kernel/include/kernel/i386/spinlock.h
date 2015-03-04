/*
 * File: spinlock.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implementation of spinlocks for x86.
 */

#ifndef I386_SPINLOCK_H_
# define I386_SPINLOCK_H_

# include <kernel/arch/cpu.h>

typedef long spinlock_t;

# define SPINLOCK_INIT		1

static inline void spinlock_init(spinlock_t* lock)
{
	__asm__ volatile("movl $1, %0\n\t" /* Atomic on recent (pentium pro) */
			 : "=m" (*lock)
			 :
			 : );
}

static inline void spinlock_lock(spinlock_t* lock)
{
	__asm__ volatile("1:\n\t"
			 "lock decl %0\n\t"
			 "jz 2f\n\t" /* If 0, lock acquired */
			 "pause\n\t"
			 "jmp 1b\n\t"
			 "2:\n\t"
			 : "=m" (*lock)
			 :
			 : );
}

/**
 * Lock the spinlock. To be used on spinlocks that are used in IRQ
 * context. This function disable the IRQ before locking the lock, thus
 * avoids deadlock if an IRQ is received on a critical section.
 * Every spinlock_lock_irq() should have a correspoding
 * spinlock_unlock_irq().
 * @lock: The lock.
 * @flags: Filled with the eflags prior to the disabling of IRQ.
 */
static inline void spinlock_lock_irq(spinlock_t* lock, u32* flags)
{
	*flags = save_irq();
	disable_irq();
	spinlock_lock(lock);
}

static inline void spinlock_unlock(spinlock_t* lock)
{
	__asm__ volatile("movl $1, %0\n\t" /* Atomic on recent (pentium pro) */
			 : "=m" (*lock)
			 :
			 : );
}

/**
 * See spinlock_lock_irq for more information.
 * @lock: The lock.
 * @flags: Will be used to restore the CPU flags.
 */
static inline void spinlock_unlock_irq(spinlock_t* lock, u32 flags)
{
	spinlock_unlock(lock);
	restore_irq(flags);
}

/*
 * Try to lock and return 0 if the lock was acquired
 */
static inline int spinlock_trylock(spinlock_t* lock)
{
	int ret = 0;

	__asm__ volatile("lock decl %0\n\t"
			 "setnz %b1\n\t"
			 : "=m" (*lock), "=r" (ret)
			 : "1" (ret)
			 : );

	return ret;
}

#endif /* !I386_SPINLOCK_H_ */
