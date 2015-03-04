#ifndef CLONE_H_
# define CLONE_H_

enum clone_flags {
	CLONE_FORK = 0,
	CLONE_MEM = 1,
	CLONE_FDT = 2,
	CLONE_FS = 4,
	CLONE_SCHED = 8
};

#endif /* !CLONE_H_ */
