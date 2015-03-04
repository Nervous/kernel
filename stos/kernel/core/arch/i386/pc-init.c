#include <kernel/klog.h>
#include <kernel/stos.h>
#include "display.h"

void arch_init(void* archdata)
{
	init_display();
}
