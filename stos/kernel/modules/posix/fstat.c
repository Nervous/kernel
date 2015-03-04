/*
 * File: stat.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement *stat syscall.
 */

#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/stat.h>
#include <kernel/string.h>
#include <kernel/syscall.h>
#include <kernel/types.h>
#include <kernel/user.h>
#include <kernel/vfs.h>

#include <kernel/fd_handler.h>

long __syscall sys_fstat(int fd, struct stat* buf)
{
	if (!is_valid_fd(fd))
		return -EBADF;

	struct file* f = file_get(fd);
	struct inode* ino = f->f_inode;

	struct stat kbuf;
	memset(&kbuf, 0, sizeof(struct stat));

	kbuf.st_ino = ino->i_ino;
	kbuf.st_mode = ino->i_mode;
	kbuf.st_uid = ino->i_uid;
	kbuf.st_gid = ino->i_gid;
	kbuf.st_size = ino->i_size;

	int ret = write_user_buf(buf, &kbuf, sizeof(struct stat));
	refcnt_dec(&f->f_refcnt);

	return ret;
}
