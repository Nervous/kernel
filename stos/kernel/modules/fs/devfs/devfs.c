#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/fs.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/stat.h>
#include <kernel/string.h>
#include <kernel/vfs.h>

struct super* super = NULL;

static struct super* devfs_get_sb(struct fs_partition* part)
{
	return super;
}

static int devfs_kill_sb(struct super* sb)
{
	return 0;
}

static struct inode* devfs_alloc_inode(struct super* sb)
{
	struct inode* res = kmalloc(sizeof (struct inode));
	if (res == NULL)
		return make_err_ptr(-ENOMEM);
	memset(res, 0, sizeof (struct inode));
	res->i_sb = sb;
	list_init(&res->i_brothers);
	return res;
}

static const struct super_ops devfs_sops = {
	/* Read-only FS */
	.read_inode = NULL,
	.write_inode = NULL,
	.alloc_inode = devfs_alloc_inode,
	.delete_inode = default_delete_inode,
};

static const struct file_system_ops devfs_ops = {
	.get_sb = devfs_get_sb,
	.kill_sb = devfs_kill_sb,
};

static struct file_system devfs = {
	.fs_name = "devfs",
	.fs_ops = &devfs_ops,
};

static const struct inode_ops devfs_iops;
static void __init_once devfs_init(void)
{
	/* Create and fill superblock */
	super = kmalloc(sizeof (struct super));
	if (super == NULL) {
		super = make_err_ptr(-ENOMEM);
		return;
	}
	memset(super, 0, sizeof (struct super));
	super->s_ops = &devfs_sops;

	/* Setup mode for devfs root directory */
	mode_t md = S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

	/* Create fake parent for call to vfs_create_file of root inode */
	struct inode* dummy = super->s_ops->alloc_inode(super);
	if (is_err_ptr(dummy)) {
		kfree(super, sizeof (struct super));
		super = (struct super*)dummy;
		return;
	}
	dummy->i_ops = &devfs_iops;
	dummy->i_mode = md;

	/* Create root inode */
	struct inode* inode = vfs_create_file(dummy, "/", O_DIRECTORY, md);
	if (is_err_ptr(inode)) {
		kfree(super, sizeof (struct super));
		kfree(dummy, sizeof (struct inode));
		super = (struct super*)dummy;
		return;
	}

	/* Fix '..' symbolic link */
	if (strcmp(inode->i_first_son->i_name, "..") == 0)
		inode->i_first_son->i_link_to = inode;
	else
		lentry(inode->i_first_son->i_brothers.next,
			struct inode, i_brothers)->i_link_to = inode;

	inode->i_parent = inode;
	super->s_root = inode;
	kfree(dummy, sizeof (struct inode));

	/* Register to filesystem to the vfs */
	register_fs(&devfs);
}

MODINFO {
	module_name("devfs"),
	module_init_once(devfs_init),
	module_type(M_DEVFS),
	module_deps(M_VFS | M_KMALLOC | M_LIBRARIES),
};

static struct file_ops devfs_fops = {
	.open = default_open,
	.read = default_dir_read,
	.write = NULL,
	.lseek = NULL,
	.release = default_release, /* TODO: infinite recursion? */
};

static int devfs_add_entry(struct inode* dir, struct inode* ino)
{
	if (ino->i_mode & S_IFDIR) {
		ino->i_ops = &devfs_iops;
		ino->i_fops = &devfs_fops;
	}

	return 0;
}

static const struct inode_ops devfs_iops = {
	/* devsf_lookup does the same thing as vfs' cached_lookup */
	.add_entry = devfs_add_entry,
	.lookup = NULL,
};
