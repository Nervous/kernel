#include "gdt.h"
#include "string.h"

s_gdt_entry gdt[3];
struct gdt_ptr gdt_str;

void set_entry(t_uint32 base, t_uint32 limit, t_uint8 type, int nb)
{
  s_gdt_entry tmp;
  tmp.limit_low = limit;
  tmp.baselow = base & 0xFFFF;
  tmp.base_mid = (base >> 16) & 0xFF;
  tmp.type = type & 0xF;
  tmp.limit_high = (limit >> 16) & 0xF;
  tmp.base_high = (base >> 24) & 0xFF;
  gdt[nb] = tmp;
}

void set_gdt()
{ 
  memset(gdt, 0, sizeof(s_gdt_entry) * 3);
  set_entry(0, 0xFFFFFFFF, 0x9A, 1);
  set_entry(0, 0xFFFFFFFF, 0x92, 2);
  gdt_str.base = (t_uint32) gdt;
  gdt_str.limit = sizeof(s_gdt_entry) * 3;
  gdt_flush(&gdt_str);
  switch_pe();
  reload_segs();
}


