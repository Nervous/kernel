#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/stat.h>
#include <kernel/string.h>
#include <kernel/syscall.h>
#include <kernel/user.h>
#include <kernel/vfs.h>

long __syscall sys_stat(const char* path, struct stat* buf)
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
	struct inode* ino = path_lookup(&tmp_path);
	if (is_err_ptr(ino)) {
		ret = get_err_ptr(ino);
		goto out;
	}

	struct stat kbuf;
	memset(&kbuf, 0, sizeof(struct stat));

	kbuf.st_ino = ino->i_ino;
	kbuf.st_mode = ino->i_mode;
	kbuf.st_uid = ino->i_uid;
	kbuf.st_gid = ino->i_gid;
	kbuf.st_size = ino->i_size;

	ret = write_user_buf(buf, &kbuf, sizeof(struct stat));

out:
	kfree(save, path_len);
	return ret;
}
