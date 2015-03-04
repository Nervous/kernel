CUR_DIR		:= lib/libc/unistd/

OBJS := abort.o alarm.o creat.o daemon.o eventfd.o exec.o fcntl.o \
	fnmatch.o ftime.o getcwd.o \
	gethostname.o getopt_long.o getpgrp.o getpt.o \
	initgroups.o issetugid.o \
	open.o openat.o opendir.o perror.o \
	setegid.o seteuid.o setpgrp.o setresuid.o setreuid.o \
	setuid.o sleep.o wait.o \
	time.o umount.o usleep.o isatty.o sysconf.o mkdir.o fork.o dup.o

# FIXME: isatty.c

# Not compiled: fstatfs.c ftok.c getdtablesize.c getpriority.c killpg.c
# nice.c pathconf.c popen.c pselect.c ptsname.c ptsname_r.c raise.c reboot.c
# recv.c send.c sigblock.c siginterrupt.c siglist.c signal.c signame.c
# signsetmask.c sigsuspend.c sigwait.c socketcalls.c statfs.c strsignal.c
# sysconf.c syslog.c system.c tcgetpgrp.c tcsetpgrp.c unlockpt.c
#
# Won't be included: sbrk.c brk.c mmap.c lseek64.c pread.c pwrite.c

LIBSUBDIRS	:=

include $Omk/libsubdirs.mk
