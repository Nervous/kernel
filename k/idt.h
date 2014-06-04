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

#endif/* Descripteur de segment */
