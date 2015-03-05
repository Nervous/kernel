#define ASM
#include "multiboot.h"

.section .multiboot
	.align 4
	.long	MULTIBOOT_HEADER_MAGIC
	.long	MULTIBOOT_HEADER_FLAGS
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)


.section .text
.global _start
_start:
	push %ebx
	push %eax

	call loader
