CUR_DIR		:= bin/ls/

PROG		:= ls
OBJS		:= ls.o
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

ls_CFLAGS	:= -Wall $(USER_CFLAGS) -g

PROGSUBDIRS	:=

include $Omk/prog.mk
