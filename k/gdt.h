#ifndef GDT_H
# define GDT_H
# include "kstd.h"

void switch_pe();
void reload_segs();


struct gdt_ptr
{
    t_uint16 limit;
    t_uint32 base;
} __attribute__((__packed__));

void gdt_flush(struct gdt_ptr* gdt_ptr);

struct __attribute__((__packed__)) gdt_entry
{
  t_uint16 limit_low;
  t_uint16 baselow;
  t_uint8 base_mid;
  t_uint8 type : 4;
  t_uint8 s_b : 1;
  t_uint8 dpl : 2;
  t_uint8 p_b : 1;
  t_uint8 limit_high : 4;
  t_uint8 avl : 1;
  t_uint8 l_b : 1;
  t_uint8 db_b : 1;
  t_uint8 g_b : 1;
  t_uint8 base_high;
};

typedef struct gdt_entry s_gdt_entry;

void set_gdt();

#endif /* !GDT_H */
