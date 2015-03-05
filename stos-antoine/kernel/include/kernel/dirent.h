#ifndef DIRENT_H_
# define DIRENT_H_

# include <kernel/limits.h>
# include <kernel/types.h>

struct dirent {
	__kernel_ino_t d_ino;
	__kernel_size_t d_reclen;
	unsigned char d_type;
	char d_name[NAME_MAX];
};

# define DT_SOCK	014
# define DT_LNK		012
# define DT_REG		010
# define DT_BLK		006
# define DT_DIR		004
# define DT_CHR		002
# define DT_FIFO	001
# define DT_UNKNOWN	0

#endif /* !DIRENT_H_ */
