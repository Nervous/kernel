CUR_DIR			:= kernel/modules/syscall/

OBJS			:= syscall.o syscall_handler-$(STOS_ARCH).o
MOD-$(CONFIG_SYSCALL)	:= syscall.ko

include $Omk/module.mk
