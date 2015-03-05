/*
 * File: pci.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implement the PCI bus specification.
 */

#include <kernel/arch/ioports.h>
#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/klog.h>
#include <kernel/module.h>
#include <kernel/pci.h>

#define CONFIG_ADDRESS	0xCF8
#define CONFIG_DATA	0xCFC

// #define PCI_DEBUG

static struct list_node devices = LIST_INIT(devices);

static u32 craft_pci_addr(u8 bus, u8 device, u8 function, u8 reg)
{
	return (1 << 31 | bus << 16 | device << 11 | function << 8 | reg);
}

static u32 read_pci_conf(struct pci_dev* dev, u8 reg)
{
	u32 addr = craft_pci_addr(dev->bus, dev->device , dev->function, reg);
	outl(CONFIG_ADDRESS, addr);

	u32 res = inl(CONFIG_DATA);

	return res;
}

static void write_pci_conf(struct pci_dev* dev, u8 reg, u32 val)
{
	u32 addr = craft_pci_addr(dev->bus, dev->device , dev->function, reg);
	outl(CONFIG_ADDRESS, addr);
	outl(CONFIG_DATA, val);
}

static u32 get_addr_space(struct pci_dev* dev, u8 reg, int index)
{
    u32 res = 0;
    u32 bar_val = dev->bar[index];

    write_pci_conf(dev, reg, 0xffffffff);

    res = read_pci_conf(dev, reg);
    res = ~(res & 0xfffffffc) + 1;
    dev->bar[index] = bar_val;

    return res;
}

static struct pci_dev* read_device(u8 bus, u8 device, u8 function, u32 ids)
{
	struct pci_dev* res = kmalloc(sizeof(struct pci_dev));
	if (!res)
		return res;

	res->bus = bus;
	res->device = device;
	res->function = function;

	res->device_id = ids >> 16;
	res->vendor_id = ids;

	int index = 0;

	for (int j = 0x10; j < 0x28; j += 4) {
		index = (j - 0x10) / 4;
		res->bar[index] = read_pci_conf(res, j);
		res->bar_size[index] = get_addr_space(res, j, index);
#ifdef PCI_DEBUG
		klog("pci_dev[V:0x%X, D:0x%X]: bar[%u] = 0x%X; bar_size[%u] = 0x%X\n",
		     res->vendor_id, res->device_id, index, res->bar[index],
		     index, res->bar_size[index]);
#endif
	}

	u32 intr = read_pci_conf(res, 0x3C);
	res->intr_pin = intr >> 8;
	res->intr_line = intr;

	list_init(&res->next_dev);

	return res;
}

static int enumerate_multifunction_devices(u8 bus, u8 device)
{
	for (int i = 1; i < 8; i++) {
		u32 addr = craft_pci_addr(bus, device, i, 0);
		outl(CONFIG_ADDRESS, addr);

		u32 ids = inl(CONFIG_DATA);

		if (ids == 0xFFFFFFFF)
			continue;

		struct pci_dev* dev = read_device(bus, device, i, ids);
		if (!dev)
			return -ENOMEM;


		list_insert(&devices, &dev->next_dev);
	}

	return 0;
}

static int enumerate_devices(u8 bus)
{
	for (int i = 0; i < 32; i++) {
		u32 addr = craft_pci_addr(bus, i, 0, 0);
		outl(CONFIG_ADDRESS, addr);

		u32 ids = inl(CONFIG_DATA);

		if (ids == 0xFFFFFFFF)
			continue;

		struct pci_dev* dev = read_device(bus, i, 0, ids);
		if (!dev)
			return -ENOMEM;

		list_insert(&devices, &dev->next_dev);

		u8 hdr_type = read_pci_conf(dev, 0xC) >> 16;
		if (hdr_type & 0x80) {
			int ret = enumerate_multifunction_devices(bus, i);
			if (ret < 0)
				return ret;
		}
	}

	return 0;
}

int register_pci_driver(u16 vendor, u16 device,
			const struct pci_driver_ops* ops)
{
	struct pci_dev* dev;
	lfor_each_entry(&devices, dev, next_dev) {
		if (dev->vendor_id == vendor && dev->device_id == device) {
			dev->ops = ops;
			int ret = dev->ops->attach(dev);
			if (ret < 0)
				return ret;
		}
	}
	return 0;
}
EXPORT_SYMBOL(register_pci_driver);

void pci_set_master(struct pci_dev* dev)
{
	if (dev == NULL)
		return;

	u32 cmd = read_pci_conf(dev, 0x4) | 0x4;
	write_pci_conf(dev, 0x4, cmd);
}
EXPORT_SYMBOL(pci_set_master);

static void __init_once init(void)
{
	for (int i = 0; i < 256; i++)
		enumerate_devices(i);

	struct pci_dev* dev;
	lfor_each_entry(&devices, dev, next_dev) {
		klog("PCI: %X:%X\n",
		     dev->vendor_id, dev->device_id);
	}
}
MODINFO {
	module_name("pci"),
	module_init_once(init),
	module_type(M_PCI),
	module_deps(M_KMALLOC)
};
