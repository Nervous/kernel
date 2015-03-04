#include <kernel/arch/current.h>
#include <kernel/arch/spinlock.h>
#include <kernel/block.h>
#include <kernel/errno.h>
#include <kernel/fs_partition.h>
#include <kernel/kmalloc.h>
#include <kernel/list.h>
#include <kernel/module.h>
#include <kernel/sched.h>
#include <kernel/stos.h>
#include <kernel/string.h>
#include <kernel/task.h>

/*
 * Instead of using partition. Maybe we should use a "struct file". with
 * a 64bits off_t, it should be sufficient for any sized disk.
 */

static int craft_request(struct block* blk, lba_t lba, void* buf,
			 u32 blk_cnt, enum request_type type)
{
	struct request* rq = kmalloc(sizeof(struct request));
	if (!rq)
		return -ENOMEM;

	rq->rq_lba = lba;
	rq->rq_blk_cnt = blk_cnt;
	rq->rq_buf = buf;
	rq->rq_type = type;
	rq->rq_task = get_current();

	assert(blk->b_sched);
	assert(blk->b_sched->ios_ops);
	assert(blk->b_sched->ios_ops->add_request);
	int ret = blk->b_sched->ios_ops->add_request(blk->b_sched, rq);
	if (ret < 0)
		return ret;
	//get_current()->state = TASK_SLEEPING;
	//schedule();

	/* We get there when the data were read */
	return ret;
}

static ssize_t part_file_read(struct file* f, char* buf, size_t len)
{
	struct fs_partition* part = f->f_inode->i_part;
	struct block* blk = part->p_blk;

	off_t lba = f->f_offset / blk->b_blk_size;
	int blk_count = len / blk->b_blk_size;
	if (len % blk->b_blk_size)
		blk_count++;

	void* tmp = kmalloc(blk_count * blk->b_blk_size);
	if (!tmp)
		return -ENOBUFS;

	partition_read(part, lba, tmp, blk_count);
	memcpy(buf, tmp + (f->f_offset % blk->b_blk_size), len);
	kfree(tmp, blk_count * blk->b_blk_size);

	return len;
}

static ssize_t part_file_write(struct file* f, const char* buf, size_t len)
{
	struct fs_partition* part = f->f_inode->i_part;
	struct block* blk = part->p_blk;

	off_t lba = f->f_offset / blk->b_blk_size;
	int blk_count = len / blk->b_blk_size;
	if (len % blk->b_blk_size)
		blk_count++;

	void* tmp = kmalloc(blk_count * blk->b_blk_size);
	if (!tmp)
		return -ENOBUFS;

	partition_read(part, lba, tmp, blk_count);
	memcpy(tmp + (f->f_offset % blk->b_blk_size), buf, len);
	partition_write(part, lba, tmp, blk_count);
	kfree(tmp, blk_count * blk->b_blk_size);

	return len;
}

static void part_file_release(struct refcnt* ref)
{
	struct file* f = container_of(ref, struct file, f_refcnt);
	kfree(f, sizeof (struct file));
}

static const struct file_ops part_file_ops = {
	.open = default_open,
	.read = part_file_read,
	.write = part_file_write,
	.lseek = default_lseek,
	.release = part_file_release
};

static struct list_node blk_list = LIST_INIT(blk_list);
static spinlock_t blk_list_lock = SPINLOCK_INIT;

extern int io_sched_bind_block(struct block* blk, struct io_sched* sched);
int register_block_dev(struct block* blk, char* name)
{
	assert(blk);
	assert(blk->b_ops);

	/* TODO: bind with the "current" io_sched */
	int ret = io_sched_bind_block(blk, NULL);
	if (ret < 0)
		return ret;

	/* Every block device is mapped 1:1 with a partition */
	struct fs_partition* part = kmalloc(sizeof(struct fs_partition));
	if (!part)
		return -ENOMEM;
	part->p_blk = blk;
	part->p_end_lba = blk->b_lba_max;
	part->p_start_lba = 0;
	part->p_dev = register_major(NULL, name, &part_file_ops, part);
	part->p_minor_idx = 0;

	blk->b_parts = kmalloc(sizeof(struct list_node));
	if (!blk->b_parts) {
		unregister_major(part->p_dev);
		kfree(part, sizeof(struct fs_partition));
		return -ENOMEM;
	}
	list_init(blk->b_parts);
	spinlock_init(&blk->parts_lock);

	spinlock_lock(&blk_list_lock);
	list_insert(&blk_list, &blk->b_next_blk);
	spinlock_unlock(&blk_list_lock);

	blk->b_part = part;

	detect_part_table(blk);

	return 0;
}
EXPORT_SYMBOL(register_block_dev);

int blk_read(struct block* blk, lba_t lba, void* buf, u32 blk_cnt)
{
	assert(blk);
	assert(buf);

	if (lba + blk_cnt > blk->b_lba_max)
		return -EINVAL;

	return craft_request(blk, lba, buf, blk_cnt, RQ_READ);
}
EXPORT_SYMBOL(blk_read);

int blk_write(struct block* blk, lba_t lba, void* buf, u32 blk_cnt)
{
	assert(blk);
	assert(buf);

	if (lba + blk_cnt > blk->b_lba_max)
		return -EINVAL;

	return craft_request(blk, lba, buf, blk_cnt, RQ_WRITE);
}
EXPORT_SYMBOL(blk_write);

struct list_node part_table_list = LIST_INIT(part_table_list);
static spinlock_t part_table_list_lock = SPINLOCK_INIT;

int register_partition_table(struct partition_table* part_table)
{
	spinlock_lock(&part_table_list_lock);
	list_insert(&part_table_list, &part_table->pt_next);
	spinlock_unlock(&part_table_list_lock);

	struct block* blk;
	spinlock_lock(&blk_list_lock);
	lfor_each_entry(&blk_list, blk, b_next_blk) {
		if (part_table->pt_ops->detect(blk) < 0)
			continue;

		int ret = part_table->pt_ops->read_part_table(blk);
		spinlock_unlock(&blk_list_lock);
		return ret;
	}
	spinlock_unlock(&blk_list_lock);

	return 0;
}
EXPORT_SYMBOL(register_partition_table);

int unregister_partition_table(struct partition_table* part_table)
{
	spinlock_lock(&part_table_list_lock);
	list_remove(&part_table->pt_next);
	spinlock_unlock(&part_table_list_lock);
	return 0;
}
EXPORT_SYMBOL(unregister_partition_table);

int detect_part_table(struct block* blk)
{
	struct partition_table* part;
	spinlock_lock(&part_table_list_lock);
	lfor_each_entry(&part_table_list, part, pt_next) {
		if (part->pt_ops->detect(blk) < 0)
			continue;

		int ret = part->pt_ops->read_part_table(blk);
		spinlock_unlock(&part_table_list_lock);
		return ret;
	}
	spinlock_unlock(&part_table_list_lock);
	return -EINVAL;
}
EXPORT_SYMBOL(detect_part_table);

int register_fs_partition(struct block* blk, struct fs_partition* part)
{
	spinlock_lock(&blk->parts_lock);
	list_insert(blk->b_parts, &part->p_next);
	spinlock_unlock(&blk->parts_lock);

	int ret = 0;
	if ((ret = register_minor(blk->b_part->p_dev, part)) < 0)
		return ret;
	part->p_minor_idx = ret;
	/* Put a node in the VFS */

	return 0;
}
EXPORT_SYMBOL(register_fs_partition);

int unregister_fs_partition(struct block* blk, struct fs_partition* part)
{
	spinlock_lock(&blk->parts_lock);
	list_remove(&part->p_next);
	spinlock_unlock(&blk->parts_lock);

	return unregister_minor(blk->b_part->p_dev, part->p_minor_idx);
}
EXPORT_SYMBOL(unregister_fs_partition);

int partition_read(struct fs_partition* part, lba_t start, void* buf, off_t n)
{
	assert(part);
	assert(buf);

	if (n + start > part->p_end_lba)
		return -EINVAL;

	return blk_read(part->p_blk, start + part->p_start_lba, buf, n);
}
EXPORT_SYMBOL(partition_read);

int partition_write(struct fs_partition* part, lba_t start, void* buf, off_t n)
{
	assert(part);
	assert(buf);

	if (n + start > part->p_end_lba)
		return -EINVAL;

	return blk_write(part->p_blk, start + part->p_start_lba, buf, n);
}
EXPORT_SYMBOL(partition_write);

MODINFO {
	module_name("block"),
	module_type(M_BLOCK),
	module_deps(M_KMALLOC | M_IO_SCHED | M_TASK | M_DEVFS)
};
