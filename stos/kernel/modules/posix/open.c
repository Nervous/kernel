#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/fd_handler.h>
#include <kernel/kmalloc.h>
#include <kernel/string.h>
#include <kernel/syscall.h>
#include <kernel/user.h>
#include <kernel/vfs.h>

static int is_last_path_component(char* path)
{
	while (*path)
		if (*path++ == '/')
			return 0;
	return 1;
}

static int is_same_folder(char* first, char* second)
{
	return first == second || (*first == '/' && first + 1 == second);
}

long __syscall sys_open(char* path, int flags, mode_t mode)
{
	char* kpath = kmalloc(PATH_MAX);
	char* save = kpath;
	if (!kpath)
		return -ENOMEM;

	int ret = strncopy_user_buf(kpath, path, PATH_MAX);
	if (ret < 0)
		goto out;

	if (strnlen(kpath, PATH_MAX) >= PATH_MAX) {
		kfree(kpath, PATH_MAX);
		return -ENAMETOOLONG;
	}

	char* tmp_path = kpath;
	struct inode* i_file = path_lookup(&tmp_path);

	ret = -EEXIST;
	if ((flags & O_EXCL) && !is_err_ptr(i_file))
		goto out;

	if (is_err_ptr(i_file)) {
		ret = get_err_ptr(i_file);
		if (ret != -ENOENT || !(flags & O_CREAT)
		    || !is_last_path_component(tmp_path))
			goto out;

		struct inode* parent;
		if (is_same_folder(kpath, tmp_path))
			parent = get_current()->fs->pwd;
		else
			parent = path_lookup(&kpath);

		if (is_err_ptr(parent)) {
			/*
			 * I don't know if this test is needed, in fact,
			 * if the kernel is not preemptable (or SMP),
			 * the parent can't be suppressed if we get
			 * there.
			 */
			ret = get_err_ptr(i_file);
			goto out;
		}

		i_file = vfs_create_file(parent, tmp_path, flags, mode);
		if (is_err_ptr(i_file)) {
			ret = get_err_ptr(i_file);
			goto out;
		}
	}

	struct file* f_file = vfs_open(i_file, flags, mode);
	if (is_err_ptr(f_file)) {
		ret = get_err_ptr(f_file);
		goto out;
	}

	ret = file_install(f_file, 0);
	if (ret < 0)
		vfs_close(f_file);

out:
	kfree(save, PATH_MAX);
	return ret;
}
