#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/list.h>
#include <kernel/module.h>
#include <kernel/stat.h>
#include <kernel/string.h>
#include <kernel/vfs.h>

int vfs_is_empty(struct inode* ino)
{
	if (!S_ISDIR(ino->i_mode) || ino->i_first_son == NULL)
		return 1;

	struct inode* cur = ino->i_first_son;
	if (strcmp(cur->i_name, ".") && strcmp(cur->i_name, ".."))
		return 0;

	lfor_each_entry(&ino->i_first_son->i_brothers, cur, i_brothers)
		if (strcmp(cur->i_name, ".") && strcmp(cur->i_name, ".."))
			return 0;

	return 1;
}
EXPORT_SYMBOL(vfs_is_empty);

int vfs_unlink(struct inode* ino)
{
	/* Cannot remove non-empty directory */
	if (S_ISDIR(ino->i_mode) && !vfs_is_empty(ino))
		return -ENOTEMPTY;

	/* Clean-up by the fs-specific unlink function */
	int ret = 0;
	if (ino->i_parent->i_ops && ino->i_parent->i_ops->unlink)
		ret = ino->i_parent->i_ops->unlink(ino);
	if (ret < 0)
		return ret;

	/* Remove the inode from the inodes tree */
	if (list_is_empty(&ino->i_brothers))
		ino->i_parent->i_first_son = NULL;
	else if (ino->i_parent->i_first_son == ino)
		ino->i_parent->i_first_son = lentry(ino->i_brothers.next,
			struct inode, i_brothers);
	list_remove(&ino->i_brothers);

	/* Final countd- destruction */
	if (ino->i_sb->s_ops->delete_inode != NULL)
		ino->i_sb->s_ops->delete_inode(ino);
	else
		default_delete_inode(ino);
	return ret;
}
EXPORT_SYMBOL(vfs_unlink);
