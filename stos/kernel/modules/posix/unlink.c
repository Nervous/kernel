#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/string.h>
#include <kernel/syscall.h>
#include <kernel/user.h>

long __syscall sys_unlink(const char* path)
{
	size_t path_len = strlen(path) + 1;
	char* kpath = kmalloc(path_len);
	char* save = kpath;
	if (kpath == NULL)
		return -ENOMEM;

	int ret = copy_user_buf(kpath, path, path_len);
	if (ret < 0)
		goto out;

	char *tmp_path = kpath;
	struct inode* i_file = path_lookup(&tmp_path);

	if (is_err_ptr(i_file)) {
		ret = get_err_ptr(i_file);
		goto out;
	}

	ret = vfs_unlink(i_file);

out:
	kfree(save, path_len);
	return ret;
}
