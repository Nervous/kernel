/*
 * File: init.c
 * Author: Louis Feuvrier <manny@lse.epita.fr>
 *
 * Description: stos fork between the global test suite, userland, or both.
 */

#include <kernel/module.h>
#include <kernel/string.h>
#include <kernel/panic.h>
#include <kernel/test.h>
#include <kernel/start_uland.h>

char *mode_str = NULL;

void init(void)
{
	if (!mode_str)
		panic("init mode required: `init.mode={tests,uland,all}`");

	int all = !strcmp(mode_str, "all");
	if (all || !strcmp(mode_str, "tests"))
		execute_gtests();
#if 0
	if (all || !strcmp(mode_str, "uland"))
		start_uland();
#endif
	while (1);
}

MODINFO {
	module_name("init"),
	module_deps(/*M_START_ULAND | */M_TEST | M_COMMANDLINE),
	module_init_once(init),
};
module_param(mode, &mode_str);
