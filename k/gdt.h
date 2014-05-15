struct __attribute__((__packed__)) gdt_entry
{
  t_uint16 limit;
  t_uint16
}

extern void switch_pe();
extern void reload_segs();
