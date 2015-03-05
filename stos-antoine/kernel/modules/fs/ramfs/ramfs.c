/*
 * File: ramfs.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: A simple ramfs filesystem
 */
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/fs.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/stat.h>
#include <kernel/stos.h>
#include <kernel/string.h>
#include <kernel/vfs.h>
#include <kernel/user.h>

static int ramfs_open(struct inode* i, struct file* f)
{
	f->f_fs_data = i->i_fs_data;
	return 0;
}

static ssize_t ramfs_read(struct file* f, char* buf, size_t size)
{
	if (!f->f_inode)
		return -EBADF;
	if (size == 0 || !f->f_fs_data)
		return 0;

	off_t read_len = size;
	off_t max_len = f->f_inode->i_size - f->f_offset;
	if (read_len > max_len)
		read_len = max_len;

	u8* start = (u8*)f->f_fs_data + f->f_offset;
	int ret = write_user_buf(buf, start, read_len);
	if (ret)
		return ret;
	return read_len;
}

static ssize_t ramfs_write(struct file* f, const char* buf, size_t size)
{
	if (!f->f_inode)
		return -EBADF;

	size_t start_offset = 0;
	if (f->f_fs_data)
		start_offset = f->f_offset;

	off_t new_size = start_offset + size;
	if (new_size > f->f_inode->i_size) {
		u8* new_fs_data = kmalloc(new_size);
		if (!new_fs_data)
			return -ENOMEM;

		if (f->f_fs_data)
			memcpy(new_fs_data, f->f_fs_data, f->f_inode->i_size);

		int ret = copy_user_buf(new_fs_data + start_offset, buf, size);
		if (ret) {
			kfree(new_fs_data, new_size);
			return ret;
		}

		if (f->f_fs_data)
			kfree(f->f_fs_data, f->f_inode->i_size);

		f->f_fs_data = new_fs_data;
		f->f_inode->i_fs_data = f->f_fs_data;
		f->f_inode->i_size = new_size;
		return size;
	}
	else {
		int ret = copy_user_buf(f->f_fs_data + start_offset, buf, size);
		if (ret)
			return ret;
		return size;
	}
}

static void ramfs_release(struct refcnt* ref)
{
	struct file* f;
	f = container_of(ref, struct file, f_refcnt);
	kfree(f, sizeof(struct file));
}

static const struct file_ops ramfs_file_ops = {
	.open = ramfs_open,
	.read = ramfs_read,
	.write = ramfs_write,
	.lseek = default_lseek,
	.release = ramfs_release
};

/* FIXME: fill this structure */
static const struct file_ops ramfs_dir_ops = {
	.open = default_open,
	.read = default_dir_read,
	/* FIXME:
	.write = default_dir_write,
	*/
	.lseek = default_lseek,
	.release = default_release
};

static const struct inode_ops ramfs_inode_ops;

static int ramfs_add_entry(struct inode* father, struct inode* son)
{
	switch (son->i_mode & S_IFMT) {
	case S_IFREG:
		son->i_fops = &ramfs_file_ops;
		break;
	case S_IFDIR:
		son->i_ops = &ramfs_inode_ops;
		son->i_fops = &ramfs_dir_ops;
		break;
	};
	return 0;
}

static const struct inode_ops ramfs_inode_ops = {
	.add_entry = ramfs_add_entry
};

static const struct super_ops ramfs_super_ops = {
	.alloc_inode = default_alloc_inode,
	.delete_inode = default_delete_inode
};

static struct super* ramfs_get_sb(struct fs_partition* part)
{
	struct super* res = kmalloc(sizeof(struct super));
	if (!res)
		return make_err_ptr(-ENOMEM);
	memset(res, 0, sizeof(struct super));

	res->s_ops = &ramfs_super_ops;

	/* Create fake parent for call to vfs_create_file of root inode */
	struct inode* dummy = res->s_ops->alloc_inode(res);
	if (is_err_ptr(dummy)) {
		kfree(res, sizeof(struct super));
		return (struct super*)dummy;
	}
	list_init(&dummy->i_brothers);
	dummy->i_ops = &ramfs_inode_ops;
	dummy->i_mode = S_IFDIR;
	dummy->i_sb = res;

	/* Create root inode */
	struct inode* inode = vfs_create_file(dummy, "/", O_DIRECTORY,
		S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP);
	if (is_err_ptr(inode))
		return (struct super*)inode;

	/* Fix '..' symbolic link */
	if (strcmp(inode->i_first_son->i_name, "..") == 0)
		inode->i_first_son->i_link_to = inode;
	else
		lentry(inode->i_first_son->i_brothers.next,
			struct inode, i_brothers)->i_link_to = inode;

	inode->i_parent = inode;
	res->s_root = inode;
	kfree(dummy, sizeof (struct inode));

	return res;
}

static const struct file_system_ops ramfs_fs_ops = {
	.get_sb = ramfs_get_sb,
};

static struct file_system ramfs_fs = {
	.fs_name = "ramfs",
	.fs_ops = &ramfs_fs_ops
};

static void __init_once init(void)
{
	register_fs(&ramfs_fs);
}

MODINFO {
	module_name("ramfs"),
	module_init_once(init),
	module_deps(M_VFS | M_KMALLOC)
};
