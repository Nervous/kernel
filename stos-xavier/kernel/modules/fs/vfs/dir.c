/*
 * File: dir.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */
#include <kernel/dirent.h>
#include <kernel/errno.h>
#include <kernel/module.h>
#include <kernel/limits.h>
#include <kernel/string.h>
#include <kernel/types.h>
#include <kernel/vfs.h>

size_t fill_dirent(struct inode* ino, struct dirent* dir, size_t* size)
{
	size_t reclen = offsetof(struct dirent, d_name) + ino->i_name_len + 1;
	reclen = align2_up(reclen, 4);
	if (reclen > *size)
		return 0;

	dir->d_ino = ino->i_ino;
	dir->d_reclen = reclen;
	dir->d_type = ino->i_mode;
	strcpy(dir->d_name, ino->i_name);

	*size -= reclen;

	return reclen;
}
EXPORT_SYMBOL(fill_dirent);

ssize_t default_dir_read(struct file* f, char* buf, size_t size)
{
	if (f->f_offset && !f->f_fs_data)
		return 0;

	struct inode* ino = f->f_inode;

	if (!ino->i_first_son)
		return 0;

	int ret = 0;
	struct dirent* dir = (void*)buf;

	if (!f->f_fs_data)
		f->f_fs_data = ino->i_first_son;

	struct inode* son = f->f_fs_data;
	do {
		size_t inc = fill_dirent(son, dir, &size);
		if (!inc) {
			if (!ret)
				return -EINVAL;
			f->f_fs_data = son;
			return ret;
		}

		ret += inc;

		dir = (void*)(char*)dir + dir->d_reclen;
		son = lentry(son->i_brothers.next, struct inode, i_brothers);

	} while (son != ino->i_first_son);

	f->f_fs_data = NULL;
	return ret;
}
EXPORT_SYMBOL(default_dir_read);
