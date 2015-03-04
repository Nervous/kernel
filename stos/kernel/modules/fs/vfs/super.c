/*
 * File: super.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implements default super operations.
 */

#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/string.h>
#include <kernel/task.h>
#include <kernel/vfs.h>

struct inode* default_alloc_inode(struct super* sb)
{
	struct inode* res = kmalloc(sizeof(struct inode));
	if (!res)
		return make_err_ptr(-ENOMEM);
	memset(res, 0, sizeof(struct inode));

	res->i_uid = get_current()->uid;
	res->i_gid = get_current()->gid;

	res->i_sb = sb;

	list_init(&res->i_brothers);

	return res;
}
EXPORT_SYMBOL(default_alloc_inode);

void default_delete_inode(struct inode* i)
{
	if (i->i_name != NULL)
		kfree(i->i_name, strlen(i->i_name) + 1);
	kfree(i, sizeof(struct inode));
}
EXPORT_SYMBOL(default_delete_inode);
