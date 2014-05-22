.global switch_pe
switch_pe:
    movl %cr0, %eax
    or $1, %al
    movl %eax, %cr0 /* Switching to protected mode */
    ret

.global reload_segs
reload_segs:
    ljmp $0x08, $1f /* long jump to code */
    ret

1:
    mov $0x10, %ax /* Third entry of gdt is kernel data */
    mov %ax, %ds
    mov %ax, %ss
    ret

.global gdt_flush
gdt_flush:
    mov 0x4(%esp), %eax
    lgdt (%eax)
    ret
