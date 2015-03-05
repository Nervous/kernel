/*
 * File: mman.c
 * Author: Gabriel Laskar <gabriel@lse.epita.fr>
 *
 * Description: Implement the memory related syscalls.
 */

#include <kernel/arch/current.h>
#include <kernel/arch/page.h>
#include <kernel/errno.h>
#include <kernel/mem.h>
#include <kernel/syscall.h>
#include <kernel/task.h>

long __syscall sys_mmap(void* addr, size_t len, int prot, int flags,
			int filedes, off_t off)
{
	assert(filedes == -1);

	len = align2_up(len, PAGE_SIZE);

	addr = find_free_mem_range(get_current(), len);

	int ret = add_mem_zone(get_current(), addr, addr + len);
	if (ret < 0)
		return ret;

	return (long)addr;
}

long __syscall sys_munmap(void* addr, size_t len)
{
	/* FIXME */
	return -ENOSYS;
}

long __syscall sys_mprotect(void* addr, size_t len, int prot)
{
	/* FIXME */
	return -ENOSYS;
}
