CUR_DIR		:= bin/init/

PROG		:= init
OBJS		:= init.o
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

init_CFLAGS	:= -Wall $(USER_CFLAGS) -g

PROGSUBDIRS	:=

include $Omk/prog.mk
