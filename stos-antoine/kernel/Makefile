CUR_DIR	:= kernel/

PROG		:= stos
OBJS		:=
LIBS		:= libcompiler-rt
PROGSUBDIRS	:= core/ modules/
MODULES		:=
INSTALL_DIR	:= root/kernel

stos_CFLAGS		:= -std=gnu99 -nostdinc -fno-builtin -fno-common
stos_CFLAGS		+= -fno-asynchronous-unwind-tables
stos_CFLAGS		+= -ffreestanding
stos_CFLAGS		+= -Wall -Wextra -Wno-unused-parameter -Wno-attributes
stos_CFLAGS		+= -DMAJOR=$(STOS_MAJOR) -DMINOR=$(STOS_MINOR)
stos_CFLAGS		+= -DARCH=\"$(STOS_ARCH)\" -DMACH=\"$(STOS_MACH)\"
stos_CFLAGS		+= -DGIT_HEAD=\"$(STOS_GIT_HEAD)\" -D__KERNEL__
stos_CFLAGS		+= -I$Oinclude/ -I$Okernel/include -Iinclude/

stos_ASFLAGS		:= -I$Oinclude/ -I$Okernel/include -Iinclude/
stos_LDFLAGS		:=

$(CUR_DIR)stos: stos-config

include $Omk/prog.mk
