#ifndef IDT_HH
# define IDT_HH
    //void flud_idt(STRUCT_IDT_ADDR idt_ptr);

# define INT_GATE 0x8E00
# define TRAP_GATE 0xEF00

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
void put_err_code();
void int_ret();
void init_pic();
void unmask_master(t_uint8 pic);
void unmask_slave(t_uint8 pic);
int get_err_code();

#endif/* Descripteur de segment */
