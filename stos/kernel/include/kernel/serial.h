#ifndef SERIAL_H_
# define SERIAL_H_

/**
 * register_serial_device - Register a serial device as a char device.
 *
 * @port: base address ioport
 * @irq:  irq number
 *
 * Return:
 *   idx on success
 */
int register_serial_device(u16 port, u8 irq);

#endif /* !SERIAL_H_ */
