/*
 * File: test.c
 * Author: Louis Feuvrier <manny@lse.epita.fr>
 *
 * Description: stos global and unit tests.
 */

#include <kernel/errno.h>
#include <kernel/module.h>
#include <kernel/list.h>
#include <kernel/arch/spinlock.h>
#include <kernel/kmalloc.h>
#include <kernel/strutils.h>
#include <kernel/string.h>
#include <kernel/test.h>

static struct list_node gtests = LIST_INIT(gtests);
static spinlock_t gtests_lock = SPINLOCK_INIT;

int register_gtest(const char *name, void (*test)(struct gtest*), void *data,
	void (*failure)(struct gtest*))
{
	struct gtest *gtest = kmalloc(sizeof(struct gtest));
	if (!gtest)
		return -ENOMEM;
	memset(gtest, 0, sizeof(struct gtest));

	gtest->name = strdup(name);
	gtest->failure = failure;
	gtest->test = test;
	gtest->data = data;

	list_init(&gtest->next);
	spinlock_lock(&gtests_lock);
	list_insert(&gtests, &gtest->next);
	spinlock_unlock(&gtests_lock);

	return 0;
}
EXPORT_SYMBOL(register_gtest);

void execute_gtests(void)
{
	struct gtest *gtest;
	spinlock_lock(&gtests_lock);
	lfor_each_entry(&gtests, gtest, next) {
		gtest->test(gtest);
		if (gtest->res && gtest->failure)
			gtest->failure(gtest);
	}
	spinlock_unlock(&gtests_lock);
}
EXPORT_SYMBOL(execute_gtests);

MODINFO {
	module_name("test"),
	module_type(M_TEST),
	module_deps(M_KMALLOC | M_LIBRARIES),
};
