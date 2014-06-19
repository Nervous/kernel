#include "idt.h"
#include "idt_func.h"

struct idtdesc idt[255];
struct idtr idtr;

void unmask_PIT();

static void init_idt_dec(t_uint16 select, t_uint32 offset, t_uint16 type, struct idtdesc *desc)
{
  desc->offset0_15 = (offset & 0xffff);
  desc->select = select;
  desc->type = type;
  desc->offset16_31 = (offset & 0xffff0000) >> 16;
  return;
}

/*
 * Cette fonction initialise la IDT apres que le kernel soit charge 
 * en memoire. 
 */
void init_idt(void)
{
  /* Initialisation des descripteurs systeme par defaut */
  init_idt_dec(0x08, (t_uint32) handler_0, INT_GATE, &idt[0]);
  init_idt_dec(0x08, (t_uint32) handler_1, INT_GATE, &idt[1]);
  init_idt_dec(0x08, (t_uint32) handler_2, INT_GATE, &idt[2]);
  init_idt_dec(0x08, (t_uint32) handler_3, INT_GATE, &idt[3]);
  init_idt_dec(0x08, (t_uint32) handler_4, INT_GATE, &idt[4]);
  init_idt_dec(0x08, (t_uint32) handler_5, INT_GATE, &idt[5]);
  init_idt_dec(0x08, (t_uint32) handler_6, INT_GATE, &idt[6]);
  init_idt_dec(0x08, (t_uint32) handler_7, INT_GATE, &idt[7]);
  init_idt_dec(0x08, (t_uint32) handler_8, INT_GATE, &idt[8]);
  init_idt_dec(0x08, (t_uint32) handler_9, INT_GATE, &idt[9]);
  init_idt_dec(0x08, (t_uint32) handler_10, INT_GATE, &idt[10]);
  init_idt_dec(0x08, (t_uint32) handler_11, INT_GATE, &idt[11]);
  init_idt_dec(0x08, (t_uint32) handler_12, INT_GATE, &idt[12]);
  init_idt_dec(0x08, (t_uint32) handler_13, INT_GATE, &idt[13]);
  init_idt_dec(0x08, (t_uint32) handler_14, INT_GATE, &idt[14]);


    init_idt_dec(0x08, (t_uint32) isr_timer, INT_GATE, &idt[64]); /* horloge */

    init_idt_dec(0x08, (t_uint32) isr_keyboard, INT_GATE, &idt[65]);	/* clavier */

  idtr.limite = 255 * 8 - 1;
  idtr.base = (t_uint32) idt;
  idt_flush(&idtr);
  init_pic();
  init_pit();
  //mask_all_IRQ();
  //unmask_master(0);
}
