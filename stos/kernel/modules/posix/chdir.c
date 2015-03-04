/*
 * File: chdir.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement chdir() syscall.
 */

#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/limits.h>
#include <kernel/syscall.h>
#include <kernel/user.h>
#include <kernel/vfs.h>

long __syscall sys_chdir(const char* path)
{
	char* kpath = kmalloc(PATH_MAX);
	if (!kpath)
		return -ENOMEM;

	char* kpath_sav = kpath;

	int ret = copy_user_buf(kpath, path, PATH_MAX);
	if (ret < 0)
		goto out;

	struct inode* dir = path_lookup(&kpath);
	if (is_err_ptr(dir)) {
		ret = get_err_ptr(dir);
		goto out;
	}

	if (!S_ISDIR(dir->i_mode)) {
		ret = -ENOTDIR;
		kfree(dir, sizeof(struct inode));
		goto out;
	}

	ret = 0;
	get_current()->fs->pwd = dir;

out:
	kfree(kpath_sav, PATH_MAX);
	return ret;
}
