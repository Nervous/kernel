#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/fd_handler.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/string.h>
#include <kernel/task.h>
#include <kernel/types.h>
#include <kernel/vfs.h>

#define MAX_FDS		256

int file_install(struct file* file, uword min)
{
	int ret;
	struct filedesc* fds = get_current()->fds;
	assert(fds);

	spinlock_lock(&fds->files_lock);
	for (ret = min; ret < fds->max_fds; ret++)
		if (!fds->files[ret])
			break;
	if (ret == fds->max_fds) {
		spinlock_unlock(&fds->files_lock);
		return -EMFILE;
	}
	fds->files[ret] = file;
	spinlock_unlock(&fds->files_lock);
	return ret;
}
EXPORT_SYMBOL(file_install);

/*
 * Assume that fd is valid
 */
struct file* file_get(int fd)
{
	assert(get_current()->fds);
	spinlock_lock(&get_current()->fds->files_lock);
	struct file* res = get_current()->fds->files[fd];
	refcnt_inc(&res->f_refcnt);
	spinlock_unlock(&get_current()->fds->files_lock);

	return res;
}
EXPORT_SYMBOL(file_get);

void file_release(int fd)
{
	struct file* f = file_get(fd);

	vfs_close(f);

	spinlock_lock(&get_current()->fds->files_lock);
	refcnt_dec(&f->f_refcnt);
	get_current()->fds->files[fd] = NULL;
	spinlock_unlock(&get_current()->fds->files_lock);
}
EXPORT_SYMBOL(file_release);

int is_valid_fd(int fd)
{
	struct filedesc* fds = get_current()->fds;

	return (fd >= 0 && fd < fds->max_fds && fds->files[fd]);
}
EXPORT_SYMBOL(is_valid_fd);

int install_fd_table(struct task* t, int size)
{
	struct filedesc* fds = kmalloc(sizeof(struct filedesc));
	if (!fds)
		return -ENOMEM;

	fds->files = kmalloc(sizeof(struct file*) * size);
	if (!fds->files) {
		kfree(fds, sizeof(struct filedesc));
		return -ENOMEM;
	}
	memset(fds->files, 0, size * sizeof(struct file*));

	fds->max_fds = size;
	spinlock_init(&fds->files_lock);

	t->fds = fds;

	return 0;
}
EXPORT_SYMBOL(install_fd_table);
