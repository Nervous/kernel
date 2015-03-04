/*
 * File: types.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#ifndef I386_TYPES_H
# define I386_TYPES_H

typedef unsigned int		__uword;
typedef int			__sword;

typedef unsigned char		__u8;
typedef signed char		__s8;
typedef unsigned short		__u16;
typedef signed short		__s16;
typedef unsigned int		__u32;
typedef signed int		__s32;
typedef unsigned long long	__u64;
typedef signed long long	__s64;

typedef __u8			__uint8_t;
typedef __s8			__int8_t;
typedef __u16			__uint16_t;
typedef __s16			__int16_t;
typedef __u32			__uint32_t;
typedef __s32			__int32_t;
typedef __s64			__int64_t;
typedef __u64			__uint64_t;

/* Most of those define were stolen from Linux. */

typedef unsigned long		__kernel_blkcnt_t;
typedef unsigned long		__kernel_blksize_t;

typedef unsigned int		__kernel_fsblkcnt_t;
typedef unsigned int		__kernel_fsfilcnt_t;
typedef unsigned int		__kernel_id_t;

typedef unsigned long		__kernel_dev_t;
typedef unsigned long		__kernel_ino_t;
typedef unsigned int		__kernel_mode_t;
typedef unsigned short		__kernel_nlink_t;
typedef signed int		__kernel_off_t;
typedef int			__kernel_pid_t;
typedef unsigned short		__kernel_ipc_pid_t;
typedef unsigned int		__kernel_size_t;
typedef int			__kernel_ssize_t;
typedef int			__kernel_ptrdiff_t;
typedef long			__kernel_time_t;
typedef long			__kernel_suseconds_t;
typedef long			__kernel_clock_t;
typedef int			__kernel_timer_t;
typedef int			__kernel_clockid_t;
typedef int			__kernel_daddr_t;
typedef char*			__kernel_caddr_t;
typedef unsigned int		__kernel_uid_t;
typedef unsigned int		__kernel_gid_t;
/* Type of a SYSV IPC key. */
typedef int			__kernel_key_t;
typedef int			__kernel_mqd_t;

typedef long long		__kernel_loff_t;

#endif /* !I386_TYPES_H */
