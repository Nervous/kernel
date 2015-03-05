CUR_DIR	:= kernel/modules/

# You _should_ add themed subfolders of modules here
SUBDIRS	:= \
	arch/$(STOS_ARCH)/ \
	dev/ \
	drivers/ \
	fs/ \

# You _should_ add standard single modules here
SUBDIRS	+= \
	bin_elf/ \
	block/ \
	char/ \
	commandline/ \
	fifo_io_sched/ \
	first_fit_kmalloc/ \
	frame_allocator/ \
	libraries/ \
	posix/ \
	start_uland/ \
	test/ \
	syscall/ \
	syslog/ \

# You _could_ add test modules here
SUBDIRS	+=

# You _must_ add the last init module here
SUBDIRS	+= \
	init/

include $Omk/subdirs.mk
