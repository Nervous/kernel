/*
 * File: syscall.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#include <kernel/arch/current.h>
#include <kernel/errno.h>
#include <kernel/interrupts.h>
#include <kernel/mem.h>
#include <kernel/module.h>
#include <kernel/stos.h>
#include <kernel/string.h>
#include <kernel/syscall.h>
#include <kernel/user.h>

#define SYSCALL_GATE	0x80U

syscall_handler_t syscall_table[NR_SYSCALLS];

int register_syscall(syscall_handler_t handler, int syscall_num)
{
	if (syscall_table[syscall_num])
		return -EINVAL;

	syscall_table[syscall_num] = handler;

	return 0;
}
EXPORT_SYMBOL(register_syscall);

int copy_user_buf(void* kbuf, const void* ubuf, size_t n)
{
	if (!is_addr_range_valid(get_current(), ubuf, ubuf + n))
		return -EFAULT;

	memcpy(kbuf, ubuf, n);

	return 0;
}
EXPORT_SYMBOL(copy_user_buf);

int strncopy_user_buf(void* kbuf, const void* ubuf, size_t n)
{
	if (!is_addr_range_valid(get_current(), ubuf, ubuf))
		return -EFAULT;

	strncpy(kbuf, ubuf, n);

	return 0;
}
EXPORT_SYMBOL(strncopy_user_buf);

int write_user_buf(void* ubuf, const void* kbuf, size_t n)
{
	if (!is_addr_range_valid(get_current(), ubuf, ubuf + n))
		return -EFAULT;

	memcpy(ubuf, kbuf, n);

	return 0;
}
EXPORT_SYMBOL(write_user_buf);

extern void syscall_entry(void);

EXPORT_SYMBOL(ret_from_syscall);

static void __init_once init(void)
{
	interrupt_add_isr((void*)syscall_entry, SYSCALL_GATE, USER_GATE);
}

MODINFO {
	module_name("syscall"),
	module_init_once(init),
	module_type(M_SYSCALL),
	module_deps(M_INTERRUPTS)
};
