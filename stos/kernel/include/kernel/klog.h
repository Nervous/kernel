/*
 * File: klog.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#ifndef KLOG_H_
# define KLOG_H_

# include <kernel/list.h>
# include <kernel/types.h>

/*
 * Chained console_ops, to be able to write to every console
 */
struct console_ops
{
	char name[8];
	int (*write)(char* buf, size_t len);
	struct list_node next_cons;
};

void klog(const char* fmt, ...);

void add_console(struct console_ops* ops);

#endif /* !KLOG_H_ */
