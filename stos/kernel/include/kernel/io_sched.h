/*
 * File: io_sched.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Interface for an io scheduler
 */
#ifndef IO_SCHED_H_
# define IO_SCHED_H_

struct block;
struct request;

struct io_sched {
	struct block* ios_blk;

	const struct io_sched_ops* ios_ops;
};

struct io_sched_ops {
	int (*add_request)(struct io_sched*, struct request*);
};

#endif /* !IO_SCHED_H_ */
