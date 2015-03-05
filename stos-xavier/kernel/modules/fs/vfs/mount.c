/*
 * File: mount.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implementation of the mount syscall.
 */

#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/fs.h>
#include <kernel/list.h>
#include <kernel/module.h>
#include <kernel/string.h>
#include <kernel/vfs.h>

struct list_node fs_head = LIST_INIT(fs_head);

static struct file_system* find_fs(char* fsname)
{
	struct file_system* fs;
	lfor_each_entry(&fs_head, fs, fs_next)
		if (!strcmp(fsname, fs->fs_name))
			return fs;

	return make_err_ptr(-ENODEV);
}

int vfs_mount(struct fs_partition* blk, struct inode* dst, char* fstype)
{
	struct file_system* fs = find_fs(fstype);
	if (is_err_ptr(fs))
		return get_err_ptr(fs);

	assert(fs->fs_ops);
	assert(fs->fs_ops->get_sb);

	struct super* sb = fs->fs_ops->get_sb(blk);
	if (is_err_ptr(sb))
		return get_err_ptr(sb);

	if (get_current()->fs->root == dst) {
		set_root_inode(sb->s_root);
	} else {
		struct inode* parent = dst->i_parent;

		/*
		 * Delete the inode we're replacing from the father.
		 */
		if (parent->i_first_son == dst) {
			if (list_is_empty(&dst->i_brothers)) {
				parent->i_first_son = NULL;
			} else {
				struct inode* new_first_son;
				new_first_son = lentry(dst->i_brothers.next,
						       struct inode, i_brothers);
				parent->i_first_son = new_first_son;
			}
		}
		list_remove(&dst->i_brothers);

		struct inode* son = parent->i_first_son;
		parent->i_first_son = sb->s_root;
		if (son)
			list_insert(&son->i_brothers, &sb->s_root->i_brothers);

		sb->s_root->i_parent = parent;

		/* TODO: names should be copied instead of traded freely */
		sb->s_root->i_name = dst->i_name;
		sb->s_root->i_name_len = dst->i_name_len;
	}

	/* We should free the cached inode in mnt_point */

	return 0;
}
EXPORT_SYMBOL(vfs_mount);

int set_root_inode(struct inode* ino)
{
	get_current()->fs->root = ino;

	return 0;
}
EXPORT_SYMBOL(set_root_inode);

int register_fs(struct file_system* fs)
{
	list_insert(&fs_head, &fs->fs_next);

	return 0;
}
EXPORT_SYMBOL(register_fs);

int unregister_fs(struct file_system* fs)
{
	list_remove(&fs->fs_next);
	return 0;
}
EXPORT_SYMBOL(unregister_fs);
