#include "serial.h"

static void outb(t_uint16 port, t_uint8 byte)
{
    __asm__ volatile("outb %0, %1\n\t"
                 :: "a" (byte), "d" (port));
}

static t_uint8 inb(t_uint16 port)
{
    t_uint8 b = 0;
    __asm__ volatile("inb %1, %0\n\t"
                     : "=&a" (b)
                     : "d" ((t_uint16) port));
    return b;
}

static void init_uart_port(t_uint16 port)
{
    outb(port + 0x01, 0x00); // Disable interrupts
    outb(port + 0x03, 0x80); // Set DLAB = 1
    outb(port + 0x00, 0x03); // low divisor = 3 (baudrate 38400)
    outb(port + 0x01, 0x00); // high divisor = 0
    outb(port + 0x03, 0x03); // 8N1
}

void init_uart()
{
    init_uart_port(COM1);
    //init_uart_port(0x2F8); // COM2
    //init_uart_port(0x3E8); // COM3
    //init_uart_port(0x2E8); // COM4
}


int send(int port, void* data, unsigned int len)
{
    t_uint8* dat = data;
    unsigned int i = 0;
    for (i = 0; i < len; ++i)
    {
        while ((inb(port + 5) & 0x20) == 0) // Check Transmitter
        {                                    // Holding Register Empty (LSR)
            if ((inb(port + 5) & 0x0A) != 0)
                return -1;
        }
        outb(port, dat[i]);
    }
    return i;
}

int c_send(int port, const void* data, unsigned int len)
{
    const t_uint8* dat = data;
    unsigned int i = 0;
    for (i = 0; i < len; ++i)
    {
        while ((inb(port + 5) & 0x20) == 0) // Check Transmitter
        {                                    // Holding Register Empty (LSR)
            if ((inb(port + 5) & 0x0A) != 0)
                return -1;
        }
        outb(port, dat[i]);
    }

    return i;
}

int recv(int port, void* data, unsigned int len)
{
    t_uint8* dat = data;
    unsigned int i = 0;
    for (i = 0; i < len; ++i)
    {
        while ((inb(port + 5) & 0x01) == 0) // Check Data Ready (LSR)
        {
            if ((inb(port + 5) & 0x0A) != 0)
                return -1;
        }

        dat[i] = inb(port);
    }

    return i;
}
