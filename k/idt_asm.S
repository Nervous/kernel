.global idt_flush
idt_flush:
    mov 0x4(%esp), %eax
    sti
    lidt (%eax)
    ret

isr:
    pop %eax
    pusha
    call *%eax
    mov $0x20, %al
    outb %al, $0x20
    popa
    iret

.global isr_keyboard
isr_keyboard:
    pushl $handler_timer
    jmp isr

.global init_pic
init_pic:
    mov $0x11, %al
    // ICW1
    outb %al, $0x20 // Master PIC
    outb %al, $0xA0 // Slave PIC
    // ICW2
    mov $0x40, %al
    outb %al, $0x21 // Master base offset = 64
    add $0x10, %al
    outb %al, $0xA1 // Slave base offset = 80
    // ICW3
    mov $0x04, %al
    outb %al, $0x21 // Slave is on PIN 2
    mov $0x02, %al
    outb %al, $0xA1
    //ICW4
    mov $0x03, %al
    outb %al, $0x21
    outb %al, $0xA1
    ret

.global unmask_master
unmask_master:
    mov 0x4(%esp), %ecx // IRQ param

    mov $1, %bl
    shl %cl, %bl // put desired bit on the correct pin
    not %bl
    inb $0x21, %al // read actual setup of pins to not override them
    and %bl, %al
    outb %al, $0x21

    //call unmask
    ret

.global unmask_slave
unmask_slave:
    mov 0x4(%esp), %eax
    push %eax
    push $0xA1

    pop %ecx // IRQ wanted
    pop %edx // Master or slave

    mov $1, %bl
    shl %cl, %bl // put desired bit on the correct pin
    inb %dx, %al // read actual setup of pins to not override them
    or %bl, %al
    outb %al, %dx

    //call unmask
    ret

.global init_pit
init_pit:
    mov $0x04, %al
    mov $0x00, %ah
    outb %al, $0x43 // Mode 2. Counter 0
    outb %al, $0x40 // Set desired frequency on counter 0
    ret

.global send_EOI_master
send_EOI_master:
    ret

.global send_EOI_slave
send_EOI_slave:
    mov $0x20, %al
    outb %al, $0xA0
    ret

.global mask_all_IRQ
mask_all_IRQ:
    mov $0xFF, %al
    outb %al, $0x21
