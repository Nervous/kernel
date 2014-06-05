#include "kstd.h"
#include "idt.h"
#include "stdio.h"

void default_int(void)
{
    printf("%s", "interrupt\r\n");
}

void handler_0(void)
{
    printf("%s", "Divide Error\r\n");
    while (1)
        ;
    int_ret();
}

void handler_1(void)
{
    printf("%s", "Reserved\r\n");
    int_ret();
}

void handler_2(void)
{
    printf("%s", "NMI Interrupt\r\n");
    int_ret();
}

void handler_3(void)
{
    printf("%s", "Breakpoint\r\n");
    int_ret();
}

void handler_4(void)
{
    printf("%s", "Overflow\r\n");
    int_ret();
}

void handler_5(void)
{
    printf("%s", "Bound Range Exceeded\r\n");
    int_ret();
}

void handler_6(void)
{
    printf("%s", "Invalid Opcode\r\n");
    int_ret();
}

void handler_7(void)
{
    printf("%s", "Devide not Available\r\n");
    
    int_ret();
}

void handler_8(void)
{
    printf("%s", "Double Fault\r\n");
    int_ret_err();
}

void handler_9(void)
{
    printf("%s", "Coprocessor Segment Overrun\r\n");
    int_ret();
}

void handler_10(void)
{
    printf("%s", "Invalid TSS\r\n");
    int_ret_err();
}

void handler_11(void)
{
    printf("%s", "Segment Not Present\r\n");
    int_ret_err();
}

void handler_12(void)
{
    printf("%s", "Stack-Segment Fault\r\n");
    int_ret_err();
}

void handler_13(void)
{
    printf("%s", "General Protection\r\n");
    int_ret_err();
}

void handler_14(void)
{
    printf("%s", "PageFault\r\n");
    int_ret_err();
}
