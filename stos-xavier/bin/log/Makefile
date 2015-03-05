CUR_DIR		:= bin/log/

PROG		:= log
OBJS		:= log.o
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

log_CFLAGS	:= -Wall $(USER_CFLAGS) -g

PROGSUBDIRS	:=

include $Omk/prog.mk
