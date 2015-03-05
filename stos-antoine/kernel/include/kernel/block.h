/*
 * File: block.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Interface of block devices.
 */
#ifndef BLOCK_H_
# define BLOCK_H_

# include <kernel/arch/spinlock.h>
# include <kernel/io_sched.h>
# include <kernel/list.h>
# include <kernel/task.h>
# include <kernel/types.h>

typedef u64 lba_t;

enum request_type {
	RQ_READ,
	RQ_WRITE
};

struct request {
	lba_t rq_lba;
	u32 rq_blk_cnt;
	void* rq_buf;

	enum request_type rq_type;

	struct task* rq_task;

	struct list_node rq_lst;
};

struct block {
	u32 b_blk_size;
	lba_t b_lba_max;

	const struct block_ops* b_ops;

	struct list_node b_request_lst;

	void* b_data;

	struct list_node b_next_blk;

	struct io_sched* b_sched;

	/* List of partition associated with this block */
	struct list_node* b_parts;
	spinlock_t parts_lock;
	/* Partition associated with this block */
	struct fs_partition* b_part;
};

struct block_ops {
	int (*request)(struct block*, struct request*);
	/* Called just before freeing the block */
	int (*release)(struct block*);
};

int register_block_dev(struct block* blk, char* name);

int blk_read(struct block* blk, lba_t lba, void* buf, u32 blk_cnt);
int blk_write(struct block* blk, lba_t lba, void* buf, u32 blk_cnt);

#endif /* !BLOCK_H_ */
