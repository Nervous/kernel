#include <kernel/device.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/stat.h>
#include <kernel/strutils.h>
#include <kernel/vfs.h>

struct major {
	struct inode* ino;

	int idx;
	int last_minor_idx;

	const struct file_ops* fops;
	void* data;
};

extern struct super* super;

struct inode* register_device(struct inode* parent, char* name,
	const struct file_ops* fops, void* device_data)
{
	if (parent != NULL && parent->i_sb != super)
		return make_err_ptr(-EINVAL);
	else if (parent == NULL)
		parent = super->s_root;

	mode_t md = S_IRWXU | S_IRWXG | S_IRWXO;
	struct inode* res = vfs_create_file(parent, name, 0, md);
	if (is_err_ptr(res))
		return res;

	res->i_ino = (ino_t)-1;
	res->i_fops = fops;
	res->i_fs_data = device_data;

	return res;
}
EXPORT_SYMBOL(register_device);

struct major* register_major(struct inode* parent, char* name,
	const struct file_ops* fops, void* device_data)
{
	static int major_idx = 0;

	if (parent != NULL && parent->i_sb != super)
		return make_err_ptr(-EINVAL);

	/* Create major device */
	struct major* maj = kmalloc(sizeof (struct major));
	if (maj == NULL)
		return make_err_ptr(-ENOMEM);
	maj->data = device_data;
	maj->last_minor_idx = 0;
	maj->idx = major_idx++;
	maj->fops = fops;

	if (parent == NULL)
		parent = super->s_root;

	/* Create major directory inode */
	mode_t md = S_IRWXU | S_IRWXG | S_IRWXO;
	struct inode* res = vfs_create_file(parent, name, O_DIRECTORY, md);
	if (is_err_ptr(res)) {
		kfree(maj, sizeof (struct major));
		return (struct major*)res;
	}
	res->i_fs_data = maj;
	res->i_ino = 0;
	maj->ino = res;

	return maj;
}
EXPORT_SYMBOL(register_major);

static size_t nbrlen(int nbr)
{
	size_t len = 1;

	do {
		len += 1;
		nbr /= 10;
	} while (nbr != 0);

	return len;
}

int register_minor(struct major* maj, void* device_data)
{
	/* Setup minor index and name */
	int idx = ++maj->last_minor_idx;
	size_t idx_len = nbrlen(idx);
	char* name = kmalloc(idx_len + 1);
	snprintf(name, idx_len, "%u", idx);

	/* Create minor inode */
	mode_t md = S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO;
	struct inode* res = vfs_create_file(maj->ino, name, 0, md);
	if (is_err_ptr(res))
		return get_err_ptr(res);
	res->i_fs_data = res->i_part = device_data;
	res->i_fops = maj->fops;
	res->i_ino = idx;

	kfree(name, idx_len + 1);
	return idx;
}
EXPORT_SYMBOL(register_minor);

int unregister_device(struct inode* device)
{
	return vfs_unlink(device);
}
EXPORT_SYMBOL(unregister_device);

int unregister_major(struct major* maj)
{
	struct inode* son = maj->ino->i_first_son;
	struct inode* cur;
	int ret = 0;

	/* Test if children inodes can be destroyed */
	if (!vfs_is_empty(son))
		return -ENOTEMPTY;
	lfor_each_entry(&son->i_brothers, cur, i_brothers)
		if (!vfs_is_empty(cur))
			return -ENOTEMPTY;

	/* Destroy children inodes
	 * Test each return value in case something bad happens */
	while (maj->ino->i_first_son != NULL)
		if ((ret = vfs_unlink(maj->ino->i_first_son)) != 0)
			return ret;

	/* Destroy the inode itself */
	if ((ret = vfs_unlink(maj->ino)) != 0)
		return ret;

	/* Destroy the device */
	kfree(maj, sizeof (struct major));
	return ret;
}
EXPORT_SYMBOL(unregister_major);

int unregister_minor(struct major* maj, int idx)
{
	struct inode* first_son = maj->ino->i_first_son;
	struct inode* target = NULL;

	/* TODO: handle sign problems */
	if (first_son->i_ino == (size_t)idx) {
		target = first_son;
	} else {
		struct inode* cur;
		lfor_each_entry_safe(&first_son->i_brothers, cur, i_brothers)
			if (cur->i_ino == (size_t)idx) {
				target = cur;
				break;
			}
	}

	if (target == NULL)
		return -ENOENT;
	return vfs_unlink(target);
}
EXPORT_SYMBOL(unregister_minor);
