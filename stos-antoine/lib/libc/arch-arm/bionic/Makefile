CUR_DIR		:= lib/libc/arch-arm/bionic/

OBJS := eabi.o exidx_dynamic.o exidx_static.o libgcc_compat.o \
	__get_pc.o __get_sp.o _exit_with_stack_teardown.o atomics_arm.o \
	crtbegin_static.o crtend.o kill.o syscall.o setjmp.o _setjmp.o tkill.o

LIBSUBDIRS	:=

include $Omk/libsubdirs.mk
