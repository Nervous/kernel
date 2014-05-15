#include "gdt.h"

void set_gdt()
{
  s_gdt_entry[6] gdt; 
  memset(gdt, 0, sizeof(s_gdt_entry) * 6);

}
