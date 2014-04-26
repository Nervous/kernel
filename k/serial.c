#include "serial.h"

static void init_uart_port(t_uint16 port)
{
    t_uint8 set_lcr = 0x83; // 8N1 & DLAB = 1
    t_uint8 set_dll = DLL;
    t_uint8 set_dlm = DLM;

    t_uint16 port_dll = port + 0x0001;
    t_uint16 port_lcr = port + 0x0003;

    __asm__ volatile("outb %0, %1\n\t"
                 :: "a" (set_dll), "d" (port_dll)); // Set DLL while DLAB=0

    __asm__ volatile("outb %0, %1\n\t"
                 :: "a" (set_lcr), "d" (port_lcr));

    __asm__ volatile("outb %0, %1\n\t"
                 :: "a" (set_dlm), "d" (port_dll)); // Set DLM now that DLAB=1
}

void init_uart()
{
    init_uart_port(0x3F8);
    init_uart_port(0x2F8);
    init_uart_port(0x3E8);
    init_uart_port(0x2E8);
}

int send(int port, void* data, unsigned int len)
{
    t_uint8* dat = data;
    for (unsigned int i = 0; i < len; ++i)
    {
        __asm__ volatile("outb %0, %1\n\t"
                         :: "a" (dat[i]), "d" ((t_uint16) port));
    }

    return len;
}

int recv(int port, void* data, unsigned int len)
{
    t_uint8* dat = data;
    for (unsigned int i = 0; i < len; ++i)
    {
        __asm__ volatile("inb %1, %0\n\t"
                         : "=&a" (dat[i])
                         : "d" ((t_uint16) port));
    }

    return len;
}
