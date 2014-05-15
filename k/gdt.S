.global switch_pe
switch_pe:
    movl $0x12, %eax
    movl %eax, %cr0 /* Switching to protected mode */

.global reload_segs
reload_segs:
    mov $0x10, %ax /* Third entry of gdt is kernel data */
    mov %ax, %ds
    mov %ax, %ss
    mov $0x08 /* Second entry of gdt is kernel code, for cs */
    ljmp %ax /* long jump to code */

.global gdt_flush
gdt_flush:
    mov 0x4(%esp), %eax
    lgdt (%eax)
