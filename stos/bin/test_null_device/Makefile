CUR_DIR		:= bin/test_null_device/

PROG		:= test_null_device
OBJS		:= test_null_device.o
LIBS		:= libc libcompiler-rt
INSTALL_DIR	:= initramfs/bin

test_null_device_CFLAGS	:= -Wall $(USER_CFLAGS) -g

PROGSUBDIRS	:=

include $Omk/prog.mk
