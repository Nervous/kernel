#ifndef SERIAL_H
# define SERIAL_H

# include "kstd.h"

# define COM1 0x03f8

int send_const(int port, const void* data, unsigned int len);
void init_uart();

#endif
