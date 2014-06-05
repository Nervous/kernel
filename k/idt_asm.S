.global idt_flush
idt_flush:
    mov 0x4(%esp), %eax
    lidt (%eax)
    ret

.global int_ret
int_ret:
    iret

.global int_ret_err
int_ret_err:
    pop %eax // Error code
    iret

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
    mov $0x02, %al
    outb %al, $0x20 // Slave is on PIN 2
    outb %al, $0xA0
    //ICW4
    mov $0x03, %al
    outb %al, $0x21
    outb %al, $0xA1
    ret

unmask:
    pop %ecx // IRQ wanted
    pop %edx // Master or slave
    mov $1, %bl
    shl %cl, %bl // put desired bit on the correct pin
    inb %dx, %al // read actual setup of pins to not override them
    or %bl, %al
    outb %al, %dx
    ret

.global unmask_master
unmask_master:
    mov 0x4(%esp), %eax // IRQ param
    push %eax
    push $0x21
    call unmask
    ret

.global unmask_slave
unmask_slave:
    mov 0x4(%esp), %eax
    push %eax
    push $0xA1
    call unmask
    ret

.global init_pit
init_pit:
    mov $0x04, %al
    mov $0x00, %ah
    outb %al, $0x43 // Mode 2. Counter 0
    outb %al, 0x40 // Set desired frequency on counter 0
