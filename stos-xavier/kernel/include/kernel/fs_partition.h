#ifndef FS_PARTITION_H_
# define FS_PARTITION_H_

# include <kernel/block.h>
# include <kernel/device.h>
# include <kernel/types.h>

struct inode;

struct fs_partition {
	struct block* p_blk;
	lba_t p_start_lba;
	lba_t p_end_lba;

	struct major* p_dev;
	int p_minor_idx;

	struct list_node p_next;
};

int register_fs_partition(struct block* blk, struct fs_partition* part);

int unregister_fs_partition(struct block* blk, struct fs_partition* part);

int partition_read(struct fs_partition* part, lba_t start, void* buf, off_t n);
int partition_write(struct fs_partition* part, lba_t start, void* buf, off_t n);

struct partition_table {
	char* pt_name;

	const struct partition_table_ops* pt_ops;

	struct list_node pt_next;
};

struct partition_table_ops {
	int (*detect)(struct block* blk);
	/* Register the partition found. */
	int (*read_part_table)(struct block* blk);
};

int register_partition_table(struct partition_table* part_table);
int unregister_partition_table(struct partition_table* part_table);
int detect_part_table(struct block* blk);

#endif /* !FS_PARTITION_H_ */
