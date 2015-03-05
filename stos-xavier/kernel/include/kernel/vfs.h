/*
 * File: vfs.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Contain the interface of the VFS.
 */
#ifndef VFS_H_
# define VFS_H_

# include <kernel/arch/spinlock.h>
# include <kernel/dirent.h>
# include <kernel/list.h>
# include <kernel/refcnt.h>
# include <kernel/types.h>

struct inode;
struct super;
struct file;
struct fs_partition;

struct inode {
	ino_t i_ino;
	uid_t i_uid;
	gid_t i_gid;
	mode_t i_mode;
	off_t i_size;

	struct super* i_sb;
	struct inode* i_parent;

	union {
		struct inode* i_first_son;
		struct inode* i_link_to;
	};

	struct list_node i_brothers;

	char* i_name;
	int i_name_len;

	const struct file_ops* i_fops;
	const struct inode_ops* i_ops;

	union {
		/* Use only when the inode is a block device */
		struct fs_partition* i_part;
	};

	void* i_fs_data; /* FS private data */
};

int set_root_inode(struct inode* ino);

struct inode_ops {
	/* Lookup the file on the dir */
	struct inode* (*lookup)(struct inode*, char*);
	/* Add an entry to the dir */
	int (*add_entry)(struct inode*, struct inode*);
	/* Unlink a file from the dir */
	int (*unlink)(struct inode*);
};

struct super {
	u32 s_blk_size;

	struct inode* s_root;

	const struct super_ops* s_ops;

	struct fs_partition* s_part;

	void* s_fs_data; /* FS private data */
};

struct super_ops {
	/* Init the inode, by reading on the disk */
	int (*read_inode)(struct super*, struct inode*);
	/* Write the inode on the disk */
	int (*write_inode)(struct inode*);
	/* Alloc an inode on the disk */
	struct inode* (*alloc_inode)(struct super*);
	/* Delete the inode from the disk */
	void (*delete_inode)(struct inode*);
	/* ... */
};

/*
 * Default super ops.
 */
struct inode* default_alloc_inode(struct super* sb);
void default_delete_inode(struct inode* i);

struct file {
	/* 32bits for an offset ? */
	off_t f_offset;
	int f_mode;
	int f_flags;
	struct inode* f_inode;

	struct refcnt f_refcnt;

	const struct file_ops* f_ops;
	void* f_fs_data;
};

struct file_ops {
	int (*open)(struct inode*, struct file*);
	ssize_t (*read)(struct file*, char*, size_t);
	ssize_t (*write)(struct file*, const char*, size_t);
	off_t (*lseek)(struct file*, off_t, int);
	/* Called just before freeing the struct file* */
	void (*release)(struct refcnt*);
	/* ... */
};

/*
 * Default file ops.
 */
int default_open(struct inode* i, struct file* f);
ssize_t default_read(struct file* f, char* buf, size_t size);
ssize_t default_dir_read(struct file* f, char* buf, size_t size);
ssize_t default_write(struct file* f, const char* buf, size_t size);
off_t default_lseek(struct file* f, off_t offset, int whence);
void default_release(struct refcnt* refcnt);

size_t fill_dirent(struct inode* ino, struct dirent* dir, size_t* size);

/*
 * Return the inode which does correspond to the path.
 * On error the path will contain the position where it failed.
 */
struct inode* path_lookup(char** path);
struct inode* vfs_lookup(struct inode* parent, char** path,
	int (*access)(struct inode*,int));

/*
 * Test if the current process has access to the inode.
 * The flag should be ORed with S_I* value.
 * Return 0 if the process has no access.
 */
int has_access(struct inode* ino, int flag);

ssize_t vfs_read(struct file* f, void* buf, size_t n);
ssize_t vfs_write(struct file* f, const void* buf, size_t n);
struct file* vfs_open(struct inode* ino, int flags, mode_t mode);
struct inode* vfs_create_file(struct inode* parent, char* name, int flags,
			      mode_t mode);
int vfs_unlink(struct inode* ino);
void vfs_close(struct file* f);
off_t vfs_lseek(struct file* f, off_t offset, int whence);
int vfs_mount(struct fs_partition* blk, struct inode* dst, char* fstype);
int vfs_add_entry(struct inode* dir, struct inode* ino);
int vfs_is_empty(struct inode* ino);

/*
 * File system information for a task.
 */
struct fs {
	struct inode* root;
	struct inode* pwd;
};

/*
 * File descriptor table
 */
struct filedesc {
	int max_fds;
	struct file** files;
	spinlock_t files_lock;
};

struct task;

int install_fd_table(struct task* t, int size);

#endif /* !VFS_H_ */
