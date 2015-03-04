/*
 * File: stat.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */
#ifndef STAT_H_
# define STAT_H_

# include <kernel/types.h>

/*
 * man 0 stat.h
 */
struct stat {
	dev_t st_dev;
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;

	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	off_t st_size;

	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;

	blksize_t st_blksize;
	blkcnt_t st_blocks;
};

# define S_IFMT		0170000

# define S_IFSOCK	0140000 /* Socket file */
# define S_IFLNK	0120000 /* Link file */
# define S_IFREG	0100000 /* Regular file */
# define S_IFBLK	0060000 /* Block device */
# define S_IFDIR	0040000 /* Dir file */
# define S_IFCHR	0020000 /* Character device */
# define S_IFIFO	0010000 /* Fifo file */

# define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
# define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
# define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
# define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
# define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
# define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
# define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)

/* File rights */
# define S_IRWXU	(S_IRUSR | S_IWUSR | S_IXUSR)
# define S_IRUSR	0400
# define S_IWUSR	0200
# define S_IXUSR	0100
# define S_IRWXG	(S_IRGRP | S_IWGRP | S_IXGRP)
# define S_IRGRP	0040
# define S_IWGRP	0020
# define S_IXGRP	0010
# define S_IRWXO	(S_IROTH | S_IWOTH | S_IXOTH)
# define S_IROTH	0004
# define S_IWOTH	0002
# define S_IXOTH	0001

# define S_ISUID	04000
# define S_ISGID	02000
# define S_ISVTX	01000

#endif /* !STAT_H_ */
