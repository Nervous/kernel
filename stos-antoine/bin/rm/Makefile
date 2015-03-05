CUR_DIR		:= bin/rm/

PROG		:= rm
OBJS		:= rm.o
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

rm_CFLAGS	:= -Wall $(USER_CFLAGS) -g

PROGSUBDIRS	:=

include $Omk/prog.mk
