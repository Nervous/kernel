#ifndef SERIAL_H
# define SERIAL_H

# define DLL 0x3
# define DLM 0x0
# include "kstd.h"

void init_uart();
int send(int port, void* data, unsigned int len);

#endif
