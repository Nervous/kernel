#ifndef GDT_H
# define GDT_H
# include "kstd.h"

extern void switch_pe();
extern void reload_segs();

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

#endif /* !GDT_H */
