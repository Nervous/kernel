#ifndef CPIO_H_
# define CPIO_H_

#include <kernel/types.h>

struct cpio_newc_header
{
	char c_magic[6];     /* Always the string "070701" for newc format */
	char c_ino[8];
	char c_mode[8];
	char c_uid[8];
	char c_gid[8];
	char c_nlink[8];
	char c_mtime[8];
	char c_filesize[8];  /* 0 for FIFOs and directories */
	char c_dev_maj[8];
	char c_dev_min[8];
	char c_rdev_maj[8];  /* Only valid for chr and blk special files */
	char c_rdev_min[8];  /* Only valid for chr and blk special files */
	char c_namesize[8];  /* Count includes terminating NUL in pathname */
	char c_chksum[8];    /* Always 0 for newc_format  */
} __attribute__((packed));

struct cpio_entry
{
	char* name;
	size_t namesize;
	u8* file;
	size_t filesize;
	unsigned int mode;
	struct cpio_newc_header* next_header;
};

void cpio_unpack_archive(void* archive_data, char* unpack_path);

#define CPIO_IRUSR 000400
#define CPIO_IWUSR 000200
#define CPIO_IXUSR 000100
#define CPIO_IRGRP 000040
#define CPIO_IWGRP 000020
#define CPIO_IXGRP 000010
#define CPIO_IROTH 000004
#define CPIO_IWOTH 000002
#define CPIO_IXOTH 000001

#define CPIO_ISUID 004000
#define CPIO_ISGID 002000
#define CPIO_ISVTX 001000

#define CPIO_ISBLK 060000
#define CPIO_ISCHR 020000
#define CPIO_ISDIR 040000
#define CPIO_ISFIFO 010000
#define CPIO_ISSOCK 0140000
#define CPIO_ISLNK 0120000
#define CPIO_ISCTG 0110000
#define CPIO_ISREG 0100000

#endif /* !CPIO_H_ */
