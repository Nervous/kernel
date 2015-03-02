AR	= ar
CAT	= cat
CC	= gcc
CP	= cp
MCOPY	= mcopy
MKFS	= mkfs
MMD	= mmd
QEMU	= qemu-system-i386
BOCHS	= bochs
RM	= rm -f

MKDIR	= mkdir
MOUNT	= mount
UMOUNT	= umount
MNT	= /mnt

CFLAGS	= -Wall -Wextra -nostdinc -fno-builtin -g3 -gdwarf-2 -m32
# SSP causes compilation problems on Ubuntu
CFLAGS	+= -fno-stack-protector -std=c99
LDFLAGS	= -nostdlib -m32 -Wl,--build-id=none
