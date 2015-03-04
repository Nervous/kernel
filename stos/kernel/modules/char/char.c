/*
 * File: char.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: character device manager.
 */

#include <kernel/arch/spinlock.h>
#include <kernel/char.h>
#include <kernel/errno.h>
#include <kernel/module.h>

static struct list_node char_lst = LIST_INIT(char_lst);
static spinlock_t char_lst_lock = SPINLOCK_INIT;

int register_char_dev(struct char_dev* chr, char* name)
{
	chr->dev = register_major(NULL, name, chr->ops, NULL);
	if (is_err_ptr(chr->dev))
		return get_err_ptr(chr->dev);

	spinlock_lock(&char_lst_lock);
	list_insert(&char_lst, &chr->next);
	spinlock_unlock(&char_lst_lock);

	return 0;
}
EXPORT_SYMBOL(register_char_dev);

MODINFO {
	module_name("char"),
	module_type(M_CHAR),
	module_deps(M_VFS | M_DEVFS)
};
