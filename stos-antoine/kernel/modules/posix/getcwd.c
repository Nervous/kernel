#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/user.h>
#include <kernel/vfs.h>

static int getcwd_rec(struct inode* ino, char* path, size_t size)
{
	int rec_ret = 0;
	if (ino->i_parent != ino) {
		rec_ret = getcwd_rec(ino->i_parent, path, size);
		if (rec_ret < 0)
			return rec_ret;
	}
	if ((unsigned int)rec_ret + ino->i_name_len + 1 > size)
		return -ERANGE;

	int res = write_user_buf(path + rec_ret, ino->i_name, ino->i_name_len);
	if (res < 0)
		return res;

	if (ino->i_parent == ino)
		return 1;

	res = write_user_buf(path + rec_ret + ino->i_name_len, "/", 1);
	if (res < 0)
		return res;

	return rec_ret + ino->i_name_len + 1;
}

long __syscall sys_getcwd(char* path, size_t size)
{
	int res = getcwd_rec(get_current()->fs->pwd, path, size);
	if (res < 0)
		return res;

	res = write_user_buf(path + res, "\0", 1);

	return res;
}
