/*
 * File: posix.c
 * Author: Gabriel Laskar <gabriel@lse.epita.fr>
 *
 * Description: Implementation of POSIX syscalls.
 */

#include <kernel/module.h>
#include <kernel/syscall.h>

static void __init_once init(void)
{
	register_syscall(sys_mmap, SYS_MMAP);
	register_syscall(sys_munmap, SYS_MUNMAP);
	register_syscall(sys_mprotect, SYS_MPROTECT);

	register_syscall(sys_open, SYS_OPEN);
	register_syscall(sys_read, SYS_READ);
	register_syscall(sys_write, SYS_WRITE);
	register_syscall(sys_close, SYS_CLOSE);
	register_syscall(sys_mount, SYS_MOUNT);
	register_syscall(sys_lseek, SYS_LSEEK);
	register_syscall(sys_fcntl, SYS_FCNTL);
	register_syscall(sys_chdir, SYS_CHDIR);
	register_syscall(sys_getcwd, SYS_GETCWD);
	register_syscall(sys_unlink, SYS_UNLINK);

	register_syscall(sys_clone, SYS_CLONE);
	register_syscall(sys_exit, SYS_EXIT);
	register_syscall(sys_fstat, SYS_FSTAT);
	register_syscall(sys_stat, SYS_STAT);
	register_syscall(sys_waitpid, SYS_WAITPID);
	register_syscall(sys_getpid, SYS_GETPID);
	register_syscall(sys_getuid, SYS_GETUID);
	register_syscall(sys_getgid, SYS_GETGID);
	register_syscall(sys_getppid, SYS_GETPPID);
}

MODINFO {
	module_name("posix"),
	module_init_once(init),
	module_deps(M_PAGINATION | M_TASK | M_SCHED | M_VFS | M_SYSCALL)
};
