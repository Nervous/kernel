#include "kstd.h"
#include "stdio.h"

void default_int(void)
{
    printf("%s", "interrupt\n");
}

void handler_0(void)
{
    printf("%s", "Divide Error");
    put_err_code();
    int_ret();
}

void handler_1(void)
{
    printf("%s", "Reserved");
    put_err_code();
    int_ret();
}

void handler_2(void)
{
    printf("%s", "NMI Interrupt");
    put_err_code();
    int_ret();
}

void handler_3(void)
{
    printf("%s", "Breakpoint");
    put_err_code();
    int_ret();
}

void handler_4(void)
{
    printf("%s", "Overflow");
    put_err_code();
    int_ret();
}

void handler_5(void)
{
    printf("%s", "Bound Range Exceeded");
    put_err_code();
    int_ret();
}

void handler_6(void)
{
    printf("%s", "Invalid Opcode");
    put_err_code();
    int_ret();
}

void handler_7(void)
{
    printf("%s", "Devide not Available");
    put_err_code();
    int_ret();
}

void handler_8(void)
{
    printf("%s", "Double Fault");
    int_ret();
}

void handler_9(void)
{
    printf("%s", "Coprocessor Segment Overrun");
    put_err_code();
    int_ret();
}

void handler_10(void)
{
    printf("%s", "Invalid TSS");
    int_ret();
}

void handler_11(void)
{
    printf("%s", "Segment Not Present");
    int_ret();
}

void handler_12(void)
{
    printf("%s", "Stack-Segment Fault");
    int_ret();
}

void handler_13(void)
{
    printf("%s", "General Protection");
    int_ret();
}

void handler_14(void)
{
    printf("%s", "PageFault");
    int_ret();
}
