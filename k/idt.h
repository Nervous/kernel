#ifndef IDT_HH
# define IDT_HH
    //void flud_idt(STRUCT_IDT_ADDR idt_ptr);

# define INT_GATE 0x8E00
# define TRAP_GATE 0xEF00
# define PIC1 0x20
# define PIC 0xA0
# define PIC1_DATA 0x21
# define PIC2_DATA 0xA1

# include "kstd.h"

struct idtdesc {
  t_uint16 offset0_15;
  t_uint16 select;
  t_uint16 type;
  t_uint16 offset16_31;
} __attribute__ ((__packed__));

/* Registre IDTR */
struct idtr {
  t_uint16 limite;
  t_uint32 base;
} __attribute__ ((__packed__));

void init_idt();
void idt_flush(struct idtr* idt_ptr);
void init_pic();
void init_pit();
void unmask_master(t_uint8 pic);
void unmask_slave(t_uint8 pic);
void isr_timer();
void isr_keyboard();
void mask_all_IRQ();

#endif/* Descripteur de segment */
