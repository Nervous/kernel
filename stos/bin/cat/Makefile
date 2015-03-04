CUR_DIR		:= bin/cat/

PROG		:= cat
OBJS		:= cat.o
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

cat_CFLAGS	:= -Wall $(USER_CFLAGS) -g

PROGSUBDIRS	:=

include $Omk/prog.mk
