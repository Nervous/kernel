#include "gdt.h"

s_gdt_entry[6] gdt;

void set_entry(t_uint32 base, t_uint16 limit, t_uint8 type, int nb)
{
  s_gdt_entry tmp;
  st.limit_low = limit
}

void set_gdt()
{
  memset(gdt, 0, sizeof(s_gdt_entry) * 6);

}


