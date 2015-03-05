CUR_DIR		:= lib/libc/

LIBSUBDIRS	:= string/ stdio/ stdlib/ bionic/ unistd/ arch-$(STOS_ARCH)/
LIBSUBDIRS	+= wchar/

LIB		:= libc
libc_CFLAGS	:= -Wall -Wextra -I$O$(CUR_DIR)/private -Wno-unused-parameter
libc_CFLAGS	:= $(libc_CFLAGS) $(USER_CFLAGS) -g -DHAVE_MORECORE=0 -DHAVE_MREMAP=0

libc_ASFLAGS	:= $(USER_ASFLAGS)

OBJS		:=

include $Omk/lib.mk
