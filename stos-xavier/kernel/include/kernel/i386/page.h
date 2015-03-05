/*
 * File: page.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */
#ifndef I386_PAGE_H_
# define I386_PAGE_H_

# define PAGE_SHIFT	12
# define PAGE_SIZE	(1UL << PAGE_SHIFT)
# define PAGE_MASK	(~(PAGE_SIZE - 1))
# define PAGE_NB	(~0UL >> PAGE_SHIFT)

# define BIG_PAGE_SHIFT	22
# define BIG_PAGE_SIZE	(1UL << BIG_PAGE_SHIFT)
# define BIG_PAGE_MASK	(~(BIG_PAGE_SIZE - 1))
# define BIG_PAGE_NB	(~0UL >> BIG_PAGE_SHIFT)

# define PDIR(P)	(((u32)P) >> 22)
# define PTABLE(P)	((((u32)P) >> 12) & 0x3ff)
# define POFFSET(P)	(((u32)P) & 0xfff)

/*
 * PDE and PTE page attributes
 */
# define P_PRESENT		1
# define P_WRITABLE		(1 << 1)
# define P_USER			(1 << 2)
# define P_WRITE_THROUGH	(1 << 3)
# define P_NO_CACHING		(1 << 4)
# define P_ACCESSED		(1 << 5)
# define P_DIRTY		(1 << 6)
# define P_4M			(1 << 7)
# define P_PAT			(1 << 7)
# define P_GLOBAL		(1 << 8)

/*
 * Generic page flags
 */
# define P_KERNEL		(P_PRESENT | P_WRITABLE)
# define P_USER_RO		(P_PRESENT | P_USER)
# define P_USER_RW		(P_PRESENT | P_WRITABLE | P_USER)

# define P_ISFLAG(Flag, Mask)	\
	((Flag) & Mask)

# define P_ISPRESENT(P)		P_ISFLAG((P), P_PRESENT)
# define P_ISWRITABLE(P)	P_ISFLAG((P), P_WRITABLE)
# define P_ISUSER(P)		P_ISFLAG((P), P_USER)
# define P_ISWRITE_THROUGH(P)	P_ISFLAG((P), P_WRITE_THROUGH)
# define P_ISNO_CACHING(P)	P_ISFLAG((P), P_NO_CACHING)
# define P_ISACCESSED(P)	P_ISFLAG((P), P_ACCESSED)
# define P_ISDIRTY(P)		P_ISFLAG((P), P_DIRTY)
# define P_IS4M(P)		P_ISFLAG((P), P_4M)
# define P_ISPAT(P)		P_ISFLAG((P), P_PAT)
# define P_ISGLOBAL(P)		P_ISFLAG((P), P_GLOBAL)

#endif /* !I386_PAGE_H_ */
