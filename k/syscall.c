#include "syscall.h"
#include "kstd.h"

void *handler_syscall(t_uint32 eax, t_uint32 ebx, t_uint32 ecx, t_uint32 edx)
{
 return syscall_table[eax - 1](ebx, ecx, edx);
}
