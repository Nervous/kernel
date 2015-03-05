/*
 * File: fifo_io_sched.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: A simple fifo scheduler
 */
#include <kernel/block.h>
#include <kernel/errno.h>
#include <kernel/io_sched.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>

static int fifo_add_request(struct io_sched* io, struct request* rq)
{
	int res = io->ios_blk->b_ops->request(io->ios_blk, rq);
	kfree(rq, sizeof(struct request));

	return res;
}

const struct io_sched_ops fifo_io_sched_ops = {
	.add_request = fifo_add_request
};

/* Shouldn't be here */
int io_sched_bind_block(struct block* blk, struct io_sched* sched)
{
	if (!sched) {
		sched = kmalloc(sizeof(struct io_sched));
		if (!sched)
			return -ENOMEM;
		sched->ios_ops = &fifo_io_sched_ops;
	}
	sched->ios_blk = blk;

	blk->b_sched = sched;

	return 0;
}
EXPORT_SYMBOL(io_sched_bind_block);

MODINFO {
	module_name("fifo_io_sched"),
	module_type(M_IO_SCHED),
	module_deps(M_KMALLOC)
};
