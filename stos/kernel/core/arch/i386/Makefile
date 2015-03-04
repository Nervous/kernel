CUR_DIR		:= kernel/core/arch/i386/

OBJS		:= btrace.o elf.o $(STOS_MACH)-display.o $(STOS_MACH)-init.o
PROGSUBDIRS	:=

include $Omk/progsubdirs.mk

stos_CFLAGS	+= -mno-mmx -mno-sse -mno-sse2
stos_LDFLAGS	:= $(stos_LDFLAGS) -Wl,-T$O$(CUR_DIR)/linker.ld
QEMU		:= qemu-system-i386 -smp 2

kernel/stos: $O$(CUR_DIR)/linker.ld
