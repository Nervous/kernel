#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/module.h>
#include <kernel/stat.h>
#include <kernel/string.h>
#include <kernel/strutils.h>
#include <kernel/task.h>
#include <kernel/vfs.h>

int vfs_add_entry(struct inode* dir, struct inode* ino)
{
	if (!S_ISDIR(dir->i_mode))
		return -ENOTDIR;

	int ret = 0;
	if (dir->i_ops && dir->i_ops->add_entry)
		ret = dir->i_ops->add_entry(dir, ino);
	if (ret < 0)
		return ret;

	ino->i_parent = dir;
	struct inode* son = dir->i_first_son;
	dir->i_first_son = ino;
	if (son)
		list_insert(&son->i_brothers, &ino->i_brothers);

	return ret;
}
EXPORT_SYMBOL(vfs_add_entry);

static struct inode* cached_lookup(struct inode* dir, char* path)
{
	assert(dir);

	if (!dir->i_first_son)
		return NULL;

	struct inode* son = dir->i_first_son;
	if (!strcmp(son->i_name, path))
		return son;

	lfor_each_entry(&dir->i_first_son->i_brothers, son, i_brothers)
		if (!strcmp(son->i_name, path))
			return son;

	return NULL;
}

int has_access(struct inode* ino, int flag)
{
	return (((ino->i_uid == get_current()->uid)
		 && (ino->i_mode & (flag & S_IRWXU)) == (flag & S_IRWXU))
		|| ((ino->i_gid == get_current()->gid)
		    && (ino->i_mode & (flag & S_IRWXG)) == (flag & S_IRWXG))
		|| (ino->i_mode & (flag & S_IRWXO)) == (flag & S_IRWXO));
}
EXPORT_SYMBOL(has_access);

struct inode* vfs_lookup(struct inode* parent, char** path,
	int (*access)(struct inode*,int))
{
	char* toksave;
	char* cur = strtok_r(*path, "/", &toksave);
	int first_iteration = 1;

	while (cur != NULL) {
		if (!S_ISDIR(parent->i_mode)) {
			*path = cur;
			return make_err_ptr(-ENOTDIR);
		}
		if (access && !access(parent, S_IXUSR | S_IXGRP | S_IXOTH)) {
			*path = cur;
			return make_err_ptr(-EACCES);
		}
		struct inode* tmp = cached_lookup(parent, cur);
		if (tmp == NULL) {
			tmp = make_err_ptr(-ENOENT);
			if (parent->i_ops && parent->i_ops->lookup)
				tmp = parent->i_ops->lookup(parent, cur);
			if (is_err_ptr(tmp)) {
				*path = cur;

				/* Restore remainder of the string */
				while ((cur = strtok_r(NULL, "/", &toksave)))
					cur[-1] = '/';

				return tmp;
			}

			/* Attach the newly created vfs inode */
			vfs_add_entry(parent, tmp);
		}

		if (S_ISLNK(tmp->i_mode))
			tmp = tmp->i_link_to;

		/* Restore slashes after use */
		if (!first_iteration)
			cur[-1] = '/';
		else
			first_iteration = 0;

		parent = tmp;
		cur = strtok_r(NULL, "/", &toksave);
	}

	return parent;
}
EXPORT_SYMBOL(vfs_lookup);

struct inode* path_lookup(char** path)
{
	struct inode* root;
	if (**path == '/')
		root = get_current()->fs->root;
	else
		root = get_current()->fs->pwd;

	return vfs_lookup(root, path, has_access);
}
EXPORT_SYMBOL(path_lookup);
