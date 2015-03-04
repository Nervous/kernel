ifeq ($(MAKELEVEL),0)
# Make is quite dumb, -rR options in MAKEFLAGS are only examined
# *before* parsing any Makefile, thus, we need to do a recursive make in
# order to enable those flags.
MAKEFLAGS	:= -rR --warn-undefined-variables --no-print-directory
ifeq ("$(origin V)", "undefined")
	V := 0
endif
ifneq ($V, 1)
	MAKEFLAGS += -s
endif
export MAKEFLAGS

all:

%:
	$(MAKE) -C . $@
else
STOS_ARCH	:= i386
STOS_MACH	:= pc
STOS_GIT_HEAD	:= 62680ea

HOST_CC		:= gcc
HOST_LD		:= gcc
HOST_AR		:= ar

HOST_CFLAGS	:=
HOST_ASFLAGS	:=

CC		:= gcc -m32
LD		:= gcc -m32
AR		:= ar
OBJCOPY		:= objcopy
STRIP		:= strip
DEFAULT_CFLAGS	:=  -g -DEARLY_KLOG_SERIAL -DEARLY_KLOG_SERIAL_PORT=0x3f8
DEFAULT_ASFLAGS	:= -D__ASSEMBLY__ 
DEFAULT_LDFLAGS	:= -nostdlib -Wl,--build-id=none 
O		:= /home/green/EPITA/Kernel/TP/stos-student/
VPATH		:= /home/green/EPITA/Kernel/TP/stos-student/

include /home/green/EPITA/Kernel/TP/stos-student/Makefile.stos

Makefile: /home/green/EPITA/Kernel/TP/stos-student/configure
	CC="gcc -m32" CFLAGS=" -g -DEARLY_KLOG_SERIAL -DEARLY_KLOG_SERIAL_PORT=0x3f8" LDFLAGS="" ASFLAGS="" \
	/home/green/EPITA/Kernel/TP/stos-student/configure --arch=i386 --mach=pc

$(DEPS:.d=.o): Makefile
endif

