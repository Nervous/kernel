/*
 * File: open.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implements open(2)
 */
#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/fd_handler.h>
#include <kernel/kmalloc.h>
#include <kernel/limits.h>
#include <kernel/module.h>
#include <kernel/stat.h>
#include <kernel/stos.h>
#include <kernel/string.h>
#include <kernel/strutils.h>
#include <kernel/user.h>
#include <kernel/vfs.h>

int default_open(struct inode* i, struct file* f)
{
	f->f_fs_data = NULL;
	return 0;
}
EXPORT_SYMBOL(default_open);

static int is_flags_valid(struct inode* ino, int flags)
{
	if (!S_ISDIR(ino->i_mode) && (flags & O_DIRECTORY))
		return -ENOTDIR;

	if (S_ISDIR(ino->i_mode) && (flags & O_WRONLY))
		return -EISDIR;

	if ((flags & O_RDONLY) && !has_access(ino, S_IRUSR))
		return -EACCES;

	if ((flags & O_WRONLY) && !has_access(ino, S_IWUSR))
		return -EACCES;

	/* Check if the FS was mounted read-only */

	return 0;
}

struct file* vfs_open(struct inode* ino, int flags, mode_t mode)
{
	int ret = is_flags_valid(ino, flags);
	if (ret < 0)
		return make_err_ptr(ret);

	struct file* f_file = kmalloc(sizeof(struct file));
	if (!f_file)
		return make_err_ptr(-ENOMEM);

	f_file->f_inode = ino;
	f_file->f_mode = mode;
	f_file->f_flags = flags;
	f_file->f_offset = 0;
	f_file->f_ops = ino->i_fops;

	refcnt_init(&f_file->f_refcnt, f_file->f_ops->release);

	assert(f_file->f_ops);

	ret = f_file->f_ops->open(ino, f_file);
	if (ret < 0) {
		vfs_close(f_file);
		return make_err_ptr(ret);
	}

	return f_file;
}
EXPORT_SYMBOL(vfs_open);

static int create_dot_files(struct inode* folder)
{
	struct super* sb = folder->i_sb;
	char* files[] = { ".", ".." };
	struct inode* links[] = { folder, folder->i_parent };

	for (int i = 0; i < 2; i++) {
		struct inode* lnk = sb->s_ops->alloc_inode(folder->i_sb);
		if (is_err_ptr(lnk)) {
			if (folder->i_first_son)
				sb->s_ops->delete_inode(folder->i_first_son);
			folder->i_first_son = NULL;
			return get_err_ptr(lnk);
		}

		lnk->i_mode = folder->i_mode & ~S_IFDIR;
		lnk->i_mode |= S_IFLNK;

		lnk->i_name = strdup(files[i]);
		if (lnk->i_name == NULL) {
			if (folder->i_first_son)
				sb->s_ops->delete_inode(folder->i_first_son);
			folder->i_first_son = NULL;
			return get_err_ptr(lnk);
		}

		lnk->i_name_len = strlen(lnk->i_name);
		lnk->i_link_to = links[i];

		int ret = vfs_add_entry(folder, lnk);
		if (ret < 0) {
			folder->i_sb->s_ops->delete_inode(lnk);
			if (folder->i_first_son)
				sb->s_ops->delete_inode(folder->i_first_son);
			folder->i_first_son = NULL;
			return ret;
		}
	}
	return 0;
}

struct inode* vfs_create_file(struct inode* parent, char* name, int flags,
			      mode_t mode)
{
	struct inode* res;

	/* FIXME: the readonly thing should be in the super */
	int ret = -EROFS;
	if (!parent->i_sb->s_ops->alloc_inode)
		return make_err_ptr(ret);

	res = parent->i_sb->s_ops->alloc_inode(parent->i_sb);
	res->i_mode = mode;
	if (flags & O_DIRECTORY)
		res->i_mode |= S_IFDIR;
	else if ((mode & S_IFMT) == 0)
		res->i_mode |= S_IFREG;

	res->i_name = strdup(name);
	if (res->i_name == NULL)
		return make_err_ptr(-ENOMEM);
	res->i_name_len = strlen(res->i_name);

	ret = vfs_add_entry(parent, res);
	if (ret < 0)
		return make_err_ptr(ret);

	if (S_ISDIR(res->i_mode) && (ret = create_dot_files(res) < 0)) {
		parent->i_sb->s_ops->delete_inode(res);
		return make_err_ptr(ret);
	}

	return res;
}
EXPORT_SYMBOL(vfs_create_file);
