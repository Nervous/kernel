CUR_DIR		:= bin/dash-0.5.6/

PROG		:= dash
OBJS		:=
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

dash_CFLAGS	:= -Wall $(USER_CFLAGS) -g -std=c99 \
		   -include $O$(CUR_DIR)/config.h -DSHELL \
		   -iquote $O$(CUR_DIR)/src -iquote $(CUR_DIR)/src

PROGSUBDIRS	:= src/

include $Omk/prog.mk
