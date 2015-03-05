#include <kernel/i386/pm.h>
#include <kernel/module.h>
#include <kernel/klog.h>
#include <kernel/stos.h>
#include <kernel/string.h>

static void init(void);

struct gdtentry {
	u16 limitlow;
	u16 baselow;
	u8 basemid;
	u8 type : 4;
	u8 s : 1;
	u8 dpl : 2;
	u8 p : 1;
	u8 limithigh : 4;
	u8 avl : 1;
	u8 l : 1;
	u8 db : 1;
	u8 g : 1;
	u8 basehigh;
} __attribute__((__packed__));
typedef struct gdtentry s_gdt_entry;

struct gdtr {
	u16 limit;
	u32 base;
} __attribute__((__packed__));
typedef struct gdtr s_gdtr;

void flush_gdt(s_gdtr* gdtr);
void set_pe_flag();
void reload_segs();
struct access {
	u8 type : 4;
	u8 s :1;
	u8 dpl : 2;
	u8 p : 1;
} __attribute__((__packed__));
typedef struct access s_access;

struct granularity {
	u8 db: 1;
	u8 g : 1;
} __attribute__((__packed__));
typedef struct granularity s_granularity;

s_gdt_entry gdt[5];
s_gdtr gdtr;

int set_gdt_gate(u32 base, u32 limit, u8 access, u8 tgranularity, int n)
{
	//granularity = g_granu + db
	//access = type + s + dpl + p, p toujours a 1
	s_gdt_entry tmp_entry;
	s_access* tmp_access = (s_access*) &access;
	s_granularity* tmp_granularity = (s_granularity*) &tgranularity;
	tmp_entry.limitlow = limit;
	tmp_entry.baselow = base & 0xFFFF;
	tmp_entry.basemid = (base >> 16) & 0xFF;
	tmp_entry.type = tmp_access->type; 
	tmp_entry.s = tmp_access->s;
	tmp_entry.dpl = tmp_access->dpl;
	tmp_entry.p = 1;
	tmp_entry.limithigh = (limit >> 16) & 0xF;
	tmp_entry.avl = 1;
	tmp_entry.db = tmp_granularity->db;
	tmp_entry.g = tmp_granularity->g;
	tmp_entry.basehigh = (base >> 24) & 0xFF;
	gdt[n] = tmp_entry;
	return 1;
}

static void reload_segments()
{
	set_cs(8);
	set_ds(16);
	set_es(16);
	set_fs(16);
	set_gs(16);
	set_ss(16);
}

static void __init_once init(void)
{
	while(1);
	klog("gdt demarrage..");
	memset(gdt, 0, sizeof(s_gdt_entry) * 5);
	set_gdt_gate(0, 0xFFFFFFFF, CODE_SEGMENT(0), GRANULARITY_4K|GRANULARITY_32bit, 1);
	set_gdt_gate(0, 0xFFFFFFFF, DATA_SEGMENT(0), GRANULARITY_4K|GRANULARITY_32bit, 2);
	set_gdt_gate(0, 0xFFFFFFFF, CODE_SEGMENT(3), GRANULARITY_4K|GRANULARITY_32bit, 3);
	set_gdt_gate(0, 0xFFFFFFFF, DATA_SEGMENT(3), GRANULARITY_4K|GRANULARITY_32bit, 4);
	gdtr.base = (u32) gdt;
	gdtr.limit = sizeof(s_gdt_entry) * 5 - 1;
	flush_gdt(&gdtr);
	set_pe_flag();
	reload_segs();
	//reload_segments();
	klog("gdt init");
}

MODINFO {
	module_name("pm"),
	module_init_once(init),
	module_type(M_MEMORY_INIT),
};	

