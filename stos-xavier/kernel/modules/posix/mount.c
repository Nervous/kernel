#include <kernel/syscall.h>
#include <kernel/errno.h>

long __syscall sys_mount(char* source, char* target, char* fstype)
{
	/*
	 * Ideally, we should just ignore the source if the fs does not need a
	 * block device.
	 */
	struct inode* blk_inode;
	if (!source) {
		blk_inode = NULL;
	} else {
		blk_inode = path_lookup(&source);
		/* TODO: check if blk_inode is a block device */
		if (is_err_ptr(blk_inode))
			return get_err_ptr(blk_inode);
	}

	struct inode* mnt_point = path_lookup(&target);
	/* TODO: check if mnt_point is a dir */
	if (is_err_ptr(mnt_point))
		return get_err_ptr(mnt_point);

	if (blk_inode)
		return vfs_mount(blk_inode->i_part, mnt_point, fstype);
	else
		return vfs_mount(NULL, mnt_point, fstype);
}
