#ifndef TYPES_H_
# define TYPES_H_

# include <kernel/arch/types.h>

typedef __uword				uword;
typedef __sword				sword;

typedef __u8				u8;
typedef __s8				s8;
typedef __u16				u16;
typedef __s16				s16;
typedef __u32				u32;
typedef __s32				s32;
typedef __u64				u64;
typedef __s64				s64;

typedef __kernel_blkcnt_t		blkcnt_t;
typedef __kernel_blksize_t		blksize_t;

typedef __kernel_fsblkcnt_t		fsblkcnt_t;
typedef __kernel_fsfilcnt_t		fsfilcnt_t;
typedef __kernel_id_t	  		id_t;

typedef __kernel_dev_t	 		dev_t;
typedef __kernel_ino_t			ino_t;
typedef __kernel_mode_t			mode_t;
typedef __kernel_nlink_t		nlink_t;
#ifndef _OFF_T_DEFINED_
#define _OFF_T_DEFINED_
typedef __kernel_off_t			off_t;
#endif
typedef __kernel_pid_t			pid_t;
typedef __kernel_ipc_pid_t		ipc_pid_t;
#ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
typedef __kernel_size_t			size_t;
#endif
#ifndef _SSIZE_T_DEFINED_
#define _SSIZE_T_DEFINED_
typedef __kernel_ssize_t		ssize_t;
#endif
typedef __kernel_time_t			time_t;
typedef __kernel_suseconds_t		suseconds_t;
typedef __kernel_clock_t		clock_t;
typedef __kernel_timer_t		timer_t;
typedef __kernel_clockid_t		clockid_t;
typedef __kernel_daddr_t		daddr_t;
typedef __kernel_caddr_t		caddr_t;
typedef __kernel_uid_t			uid_t;
typedef __kernel_gid_t 			gid_t;
typedef __kernel_key_t 			key_t;
typedef __kernel_mqd_t 			mqd_t;

typedef __kernel_loff_t			loff_t;

#endif /* !TYPES_H_ */
