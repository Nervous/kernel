#ifndef MMAN_H_
# define MMAN_H_

# define PROT_NONE	0x00
# define PROT_READ	0x01
# define PROT_WRITE	0x02
# define PROT_EXEC	0x04

# define MAP_SHARED	0x01
# define MAP_PRIVATE	0x02
# define MAP_FIXED	0x04
# define MAP_ANONYMOUS	0x08

# define MS_ASYNC	0x01
# define MS_SYNC	0x02
# define MS_INVALIDATE	0x04

# define MCL_CURRENT	0x01
# define MCL_FUTURE	0x02

#endif /* !MMAN_H_ */
