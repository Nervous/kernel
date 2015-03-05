#include <kernel/i386/pm.h>
#include <kernel/klog.h>
#include <kernel/module.h>

void switch_pe();
void reload_segs();

struct gdt_ptr
{
	u16 limit;
	u32 base;
} __attribute__((__packed__));

void gdt_flush(struct gdt_ptr* gdt_ptr);

struct __attribute__((__packed__)) gdt_entry
{
	u16 limit_low;
	u16 baselow;
	u8 base_mid;
	u8 type : 4;
	u8 s_b : 1;
	u8 dpl : 2;
	u8 p_b : 1;
	u8 limit_high : 4;
	u8 avl : 1;
	u8 l_b : 1;
	u8 db_b : 1;
	u8 g_b : 1;
	u8 base_high;
};

struct __attribute__((__packed__)) gdt_access
{
	u8 type : 4;
	u8 s_b : 1;
	u8 dpl : 2;
	u8 p_b : 1;
};

struct __attribute__((__packed__)) gdt_granularity
{
	u8 db_b : 1;
	u8 g_b : 1;
};

typedef struct gdt_access s_gdt_access;
typedef struct gdt_granularity s_gdt_granularity;
typedef struct gdt_entry s_gdt_entry;

void set_gdt();

s_gdt_entry gdt[5];
struct gdt_ptr gdt_str;

int set_gdt_gate(u32 base, u32 limit, u8 access, u8 granularity, int nb)
{
	s_gdt_granularity* gran_tmp = (s_gdt_granularity*) &granularity;
	s_gdt_access* access_tmp = (s_gdt_access*) &access;

	s_gdt_entry tmp;
	tmp.limit_low = limit;
	tmp.baselow = base & 0xFFFF;
	tmp.base_mid = (base >> 16) & 0xFF;

	tmp.avl = 1;

	tmp.g_b = gran_tmp->g_b;
	tmp.db_b = gran_tmp->db_b;


	tmp.type = access_tmp->type;
	tmp.dpl = access_tmp->dpl;
	tmp.s_b = access_tmp->s_b;
	tmp.p_b = 1;

	tmp.limit_high = (limit >> 16) & 0xF;
	tmp.base_high = (base >> 24) & 0xFF;
	gdt[nb] = tmp;
	return 1;
}


void __init_once set_gdt()
{ 
	klog("pouet\n");
	memset(gdt, 0, sizeof(s_gdt_entry) * 5);
	set_gdt_gate(0, 0xFFFFFFFF, CODE_SEGMENT(0), GRANULARITY_4K|GRANULARITY_32bit, 1);
	set_gdt_gate(0, 0xFFFFFFFF, DATA_SEGMENT(0), GRANULARITY_4K|GRANULARITY_32bit, 2);
	gdt_str.base = (u32) gdt;
	gdt_str.limit = sizeof(s_gdt_entry) * 3 - 1;
	gdt_flush(&gdt_str);
	switch_pe();
	reload_segs();
}


MODINFO {
	module_name("pm"),
	module_init_once(set_gdt),
	module_type(M_MEMORY_INIT),
};
