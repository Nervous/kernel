#ifndef REFCNT_H_
# define REFCNT_H_

# include <kernel/arch/atomic.h>

struct refcnt {
	atomic_t count;
	void (*release)(struct refcnt*);
};

static inline void refcnt_init(struct refcnt* cnt,
			       void (*release)(struct refcnt*))
{
	cnt->release = release;
	atomic_set(&cnt->count, 1);
}

/*
 * Return the value of ->count
 */
static inline atomic_t refcnt_inc(struct refcnt* cnt)
{
	return atomic_inc(&cnt->count);
}

/*
 * Return the value of ->count
 */
static inline atomic_t refcnt_dec(struct refcnt* cnt)
{
	atomic_t res = atomic_dec(&cnt->count);
	if (!res)
		cnt->release(cnt);

	return res;
}

#endif /* !REFCNT_H_ */
