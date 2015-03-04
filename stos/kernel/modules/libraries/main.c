#include <kernel/module.h>

MODINFO {
	module_name("libraries"),
	module_type(M_LIBRARIES),
	module_deps(M_KMALLOC)
};
