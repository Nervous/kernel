/*
 * File: panic.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#ifndef PANIC_H_
# define PANIC_H_

void panic(const char* msg);

void print_back_trace(void);

#endif /* !PANIC_H_ */
