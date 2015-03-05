CUR_DIR		:= lib/libc/bionic/

OBJS := arc4random.o basename.o basename_r.o \
	clearenv.o cpuacct.o dirname.o dirname_r.o dlmalloc.o \
	err.o __errno.o fdprintf.o fts.o getdents.o \
	ioctl.o libc_init_common.o \
	libc_init_static.o \
	malloc_debug_common.o \
	md5.o \
	pututline.o realpath.o \
	sched_cpualloc.o sched_cpucount.o sched_getaffinity.o sched_getcpu.o \
	__set_errno.o sha1.o system_properties.o time64.o \
	utmp.o drand48.o erand48.o

# Not compiled: if_indextoname.c if_nametoindex.c ldexp.c
# logd_write.c pthread*.c ptrace.c semaphore.c ssp.c stubs.c thread_atexit.o
# utime.c bionic_clone.o
#
# Won't be included: fork.o

LIBSUBDIRS	:=

include $Omk/libsubdirs.mk
