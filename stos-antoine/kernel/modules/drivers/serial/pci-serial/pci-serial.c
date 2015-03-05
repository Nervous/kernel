#include <kernel/klog.h>
#include <kernel/module.h>
#include <kernel/pci.h>
#include <kernel/pcidevs.h>

static struct {
	u16 vendor;
	u16 device;
} pci_serial_matchids[] = {
	{ PCI_VENDOR_REDHAT, PCI_PRODUCT_REDHAT_PCI_SERIAL },
	{ PCI_VENDOR_REDHAT, PCI_PRODUCT_REDHAT_PCI_SERIAL_DUAL },
	{ PCI_VENDOR_REDHAT, PCI_PRODUCT_REDHAT_PCI_SERIAL_QUAD },
};

int register_serial_device(u16 port, u8 irq);

static int pci_serial_attach(struct pci_dev *dev)
{
	if (dev->bar[0] & 0x1) {
		u16 base_addr = dev->bar[0] & 0xfffffffc;
		klog("port addr : %X\n", base_addr);

		switch (dev->device_id) {
			case PCI_PRODUCT_REDHAT_PCI_SERIAL_QUAD:
				register_serial_device(base_addr + (8 * 3), dev->intr_line);
				register_serial_device(base_addr + (8 * 2), dev->intr_line);
			case PCI_PRODUCT_REDHAT_PCI_SERIAL_DUAL:
				register_serial_device(base_addr + 8, dev->intr_line);
			case PCI_PRODUCT_REDHAT_PCI_SERIAL:
				register_serial_device(base_addr, dev->intr_line);
		}
	} else {
		klog("unable to use mmio bar for the moment\n");
	}
	return 0;
}

static void pci_serial_detach(struct pci_dev *dev)
{
}

static const struct pci_driver_ops pci_serial_pci_ops = {
	.attach = pci_serial_attach,
	.detach = pci_serial_detach
};

static void __init_once pci_serial_init(void)
{
	for (uword i = 0; i < array_size(pci_serial_matchids); ++i)
		register_pci_driver(pci_serial_matchids[i].vendor,
				    pci_serial_matchids[i].device,
				    &pci_serial_pci_ops);
}

MODINFO {
	module_name("pci-serial"),
	module_init_once(pci_serial_init),
	module_deps(M_PCI | M_SERIAL)
};
