CUR_DIR		:= bin/test/

PROG		:= test
OBJS		:= test.o
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

test_CFLAGS	:= -Wall $(USER_CFLAGS) -g

PROGSUBDIRS	:=

include $Omk/prog.mk
