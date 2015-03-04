CUR_DIR		:= bin/mkdir/

PROG		:= mkdir
OBJS		:= mkdir.o
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

mkdir_CFLAGS	:= -Wall $(USER_CFLAGS) -g

PROGSUBDIRS	:=

include $Omk/prog.mk
