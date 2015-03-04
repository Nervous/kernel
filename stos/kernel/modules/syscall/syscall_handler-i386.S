#include <kernel/arch/pm.h>
#include <kernel/errno.h>
#include <kernel/syscall.h>

.global syscall_entry
syscall_entry:
	pushl %eax
	pushl %ds
	movl $KERNEL_DS_SEL, %eax
	movl %eax, %ds

	pushl %fs
	movl $KERNEL_FS_SEL, %eax
	movl %eax, %fs

	movl 8(%esp), %eax

	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx

	cmpl $NR_SYSCALLS, %eax
	jae 1f
	movl syscall_table(, %eax, 4), %ebx
	cmpl $0, %ebx
	je 2f

	call *%ebx
	movl %eax, 32(%esp)
	jmp ret_from_syscall

	1:
	movl $-EINVAL, 32(%esp)
	jmp ret_from_syscall

	2:
	movl $-ENOSYS, 32(%esp)

.global ret_from_syscall
ret_from_syscall:
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popl %fs
	popl %ds
	popl %eax
	iret
