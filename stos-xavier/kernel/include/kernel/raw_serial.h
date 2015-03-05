/*
 * File: raw_serial.h
 * Authors: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Raw serial function.
 */
#ifndef RAW_SERIAL_H_
# define RAW_SERIAL_H_

void configure_serial_port(int port_nb, int rate);

/*
 * Return -1 if there's nothing to read.
 */
int serial_getchar(int port_nb);

void serial_putchar(int port_nb, char c);

#endif /* !RAW_SERIAL_H_ */
