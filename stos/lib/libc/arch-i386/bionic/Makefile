CUR_DIR		:= lib/libc/arch-i386/bionic/

OBJS := __get_sp.o __get_tls.o __set_tls.o _exit_with_stack_teardown.o \
	_setjmp.o atomics_x86.o clone.o crtbegin_static.o crtend.o setjmp.o \
	syscall.o

# Not compiled: sigsegjmp.c vfork.c

LIBSUBDIRS	:=

include $Omk/libsubdirs.mk
