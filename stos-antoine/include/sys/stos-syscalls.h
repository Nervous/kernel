/* auto-generated by gensyscalls.py, do not touch */
#ifndef _BIONIC_STOS_SYSCALLS_H_

#if !defined __ASM_ARM_UNISTD_H && !defined __ASM_I386_UNISTD_H
#if defined __arm__ && !defined __ARM_EABI__ && !defined __thumb__
  #  define __NR_SYSCALL_BASE  0x900000
  #else
  #  define  __NR_SYSCALL_BASE  0
  #endif

#define __NR_open                         (__NR_SYSCALL_BASE + 0)
#define __NR_read                         (__NR_SYSCALL_BASE + 1)
#define __NR_write                        (__NR_SYSCALL_BASE + 2)
#define __NR_close                        (__NR_SYSCALL_BASE + 3)
#define __NR_mount                        (__NR_SYSCALL_BASE + 4)
#define __NR_lseek                        (__NR_SYSCALL_BASE + 5)
#define __NR_execve                       (__NR_SYSCALL_BASE + 6)
#define __NR_clone                        (__NR_SYSCALL_BASE + 7)
#define __NR_mmap                         (__NR_SYSCALL_BASE + 8)
#define __NR_munmap                       (__NR_SYSCALL_BASE + 9)
#define __NR_mprotect                     (__NR_SYSCALL_BASE + 10)
#define __NR_exit                         (__NR_SYSCALL_BASE + 11)
#define __NR_fstat                        (__NR_SYSCALL_BASE + 12)
#define __NR_waitpid                      (__NR_SYSCALL_BASE + 13)
#define __NR_getpid                       (__NR_SYSCALL_BASE + 14)
#define __NR_getuid                       (__NR_SYSCALL_BASE + 15)
#define __NR_getgid                       (__NR_SYSCALL_BASE + 16)
#define __NR_fcntl                        (__NR_SYSCALL_BASE + 17)
#define __NR_chdir                        (__NR_SYSCALL_BASE + 18)
#define __NR_getppid                      (__NR_SYSCALL_BASE + 19)
#define __NR_getcwd                       (__NR_SYSCALL_BASE + 20)
#define __NR_unlink                       (__NR_SYSCALL_BASE + 21)
#define __NR_stat                         (__NR_SYSCALL_BASE + 22)

#ifdef __arm__
#endif

#ifdef __i386__
#endif

#if defined(__SH3__) || defined(__SH4__) 
#endif

#endif

#endif /* _BIONIC_STOS_SYSCALLS_H_ */
