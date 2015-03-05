/*
 * File: ipi.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */

#ifndef IPI_H_
# define IPI_H_

/**
 * ipi_call_fn - call a function on all online cpus
 *
 * @fn: Function to be called on each CPU.
 * @args: The argument to be given to @fn.
 *
 * Return:
 *   errno.
 */
int ipi_call_fn(void (*fn)(void* args), void* args);

/**
 * ipi_resched - Send a reschedule IPI to every CPU.
 */
void ipi_resched(void);

#endif /* !IPI_H_ */
