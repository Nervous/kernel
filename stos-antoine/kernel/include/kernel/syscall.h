#ifndef SYSCALL_H_
# define SYSCALL_H_

# define SYS_OPEN	0
# define SYS_READ	1
# define SYS_WRITE	2
# define SYS_CLOSE	3
# define SYS_MOUNT	4
# define SYS_LSEEK	5
# define SYS_EXECVE	6
# define SYS_CLONE	7
# define SYS_MMAP	8
# define SYS_MUNMAP	9
# define SYS_MPROTECT	10
# define SYS_EXIT	11
# define SYS_FSTAT	12
# define SYS_WAITPID	13
# define SYS_GETPID	14
# define SYS_GETUID	15
# define SYS_GETGID	16
# define SYS_FCNTL	17
# define SYS_CHDIR	18
# define SYS_GETPPID	19
# define SYS_GETCWD	20
# define SYS_UNLINK	21
# define SYS_STAT	22

# define NR_SYSCALLS	(SYS_STAT + 1)

# ifndef __ASSEMBLY__

#  include <kernel/stat.h>
#  include <kernel/stos.h>
#  include <kernel/task.h>
#  include <kernel/types.h>

typedef long (*syscall_handler_t)() __syscall;

int register_syscall(syscall_handler_t handler, int sys_num);

void ret_from_syscall(void);

//long __syscall sys_open(char* path, int flags, mode_t mode);
long __syscall sys_open(char* path, int flags, mode_t mode);
long __syscall sys_read(int fd, void* buf, size_t n);
long __syscall sys_write(int fd, const void* buf, size_t n);
long __syscall sys_close(int fd);
long __syscall sys_mount(char* source, char* target, char* fstype);
long __syscall sys_lseek(int fd, off_t offset, int whence);
long __syscall sys_execve(char* path, char* argv[], char* envp[]);
long __syscall sys_clone(enum clone_flags flags);
long __syscall sys_mmap(void* addr, size_t len, int prot, int flags,
			int filedes, off_t off);
long __syscall sys_munmap(void* addr, size_t len);
long __syscall sys_mprotect(void* addr, size_t len, int prot);
long __syscall sys_exit(int status);
long __syscall sys_fstat(int fd, struct stat* buf);
long __syscall sys_waitpid(pid_t pid, int* status, int options);
long __syscall sys_getpid(void);
long __syscall sys_getuid(void);
long __syscall sys_getgid(void);
long __syscall sys_fcntl(int fd, int cmd, void* arg);
long __syscall sys_chdir(const char* path);
long __syscall sys_getppid(void);
long __syscall sys_getcwd(void);
long __syscall sys_unlink(const char* path);
long __syscall sys_stat(const char* path, struct stat* buf);

# endif

#endif /* !SYSCALL_H_ */
