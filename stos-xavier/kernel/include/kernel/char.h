/*
 * File: char.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: character device interface.
 */
#ifndef CHAR_H_
# define CHAR_H_

# include <kernel/device.h>
# include <kernel/list.h>
# include <kernel/vfs.h>

struct char_dev {
	struct list_node next;
	const struct file_ops* ops;

	struct major* dev;
	int minor_idx;
};

/**
 * register_char_dev - Register and create the character device in /dev
 *
 * Devices will be created inside /dev as /dev/@name/[0-9]*
 *
 * @chr: char_dev struct to register
 * @name: name of the device
 */
int register_char_dev(struct char_dev* chr, char* name);

#endif /* !CHAR_H_ */
