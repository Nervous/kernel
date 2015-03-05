/*
 * File: irq.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Interface of an IRQ manager.
 */
#ifndef IRQ_H_
# define IRQ_H_

# include <kernel/arch/cpu.h>
# include <kernel/interrupts.h>

/*
 * An handler should return 1 if the interrupt was handled.
 */
typedef int (*irq_handler)(void* data);

void unmask_irq(int irq_nb);
void mask_irq(int irq_nb);
void mask_all_irq(void);

enum irq_flag {
	IRQ_DEFAULT = 0, /* IRQ not shared and enabled during the execution */
	IRQ_SHARED = 1, /* IRQ can be shared ? */
	/*
	 * IRQ enabled during the handler execution ? You should be very
	 * careful with this as it can cause kernel stack overflow.
	 */
	IRQ_ENABLED = 2
};

int add_irq_handler(irq_handler handler, void* data, int irq_nb,
		    enum irq_flag flags);
int add_bus_irq_handler(irq_handler handler, void* data, int irq_nb,
		    enum irq_flag flag, const char* bus, void* device_data);

void rm_irq_handler(int irq_nb);
void acknowledge_irq(int irq_nb);

#endif /* !IRQ_H_ */
