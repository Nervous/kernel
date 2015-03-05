/*
 * File: fs.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Describe a filesystem
 */
#ifndef FS_H_
# define FS_H_

# include <kernel/fs_partition.h>
# include <kernel/list.h>
# include <kernel/vfs.h>

struct file_system {
	char* fs_name;

	const struct file_system_ops* fs_ops;

	struct list_node fs_next;
};

struct file_system_ops {
	struct super* (*get_sb)(struct fs_partition*);
	int (*kill_sb)(struct super*);
};

int register_fs(struct file_system* fs);
int unregister_fs(struct file_system* fs);

#endif /* !FS_H_ */
