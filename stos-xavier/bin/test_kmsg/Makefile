CUR_DIR		:= bin/test_kmsg/

PROG		:= test_kmsg
OBJS		:= test_kmsg.o
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

test_kmsg_CFLAGS	:= -Wall $(USER_CFLAGS) -g

PROGSUBDIRS	:=

include $Omk/prog.mk
