#ifndef IDT_HH
# define IDT_HH
    //void flud_idt(STRUCT_IDT_ADDR idt_ptr);


struct idtdesc {
  u16 offset0_15;
  u16 select;
  u16 type;
  u16 offset16_31;
} __attribute__ ((__packed__));

/* Registre IDTR */
struct idtr {
  u16 limite;
  u32 base;
} __attribute__ ((__packed__));

void idt_flush(struct idtr* idt_ptr);
void put_err_code();
void int_ret();
void init_pic();
void unmask_master(t_uint8 pic);
void unmask_slave(t_uint8 pic);

#endif/* Descripteur de segment */
