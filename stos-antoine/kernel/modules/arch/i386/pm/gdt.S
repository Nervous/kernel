.global flush_gdt
flush_gdt:
	mov 0x4(%esp), %eax
	lgdt (%eax)
	ret

.global set_pe_flag
set_pe_flag:
	movl %cr0, %eax
	or $1, %al
	movl %eax, %cr0
	ret

.global reload_segs
reload_segs:
	ljmp $0x08, $1f
	ret
1:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs
	ret

