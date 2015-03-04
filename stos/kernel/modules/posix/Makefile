CUR_DIR		:= kernel/modules/posix/

OBJS		:= posix.o mman.o
OBJS		+= chdir.o close.o fd_handler.o getcwd.o lseek.o mount.o
OBJS		+= open.o read.o stat.o fstat.o write.o clone.o task.o
OBJS		+= unlink.o

MOD-$(CONFIG_POSIX_SYSCALLS) := posix.ko

include $Omk/module.mk
