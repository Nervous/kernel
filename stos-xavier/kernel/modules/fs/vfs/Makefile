CUR_DIR			:= kernel/modules/fs/vfs/

OBJS			:= close.o fd_handler.o lseek.o main.o dir.o unlink.o
OBJS			+= mount.o open.o read.o vfs_utils.o write.o super.o
MOD-$(CONFIG_VFS)	:= vfs.ko

include $Omk/module.mk
