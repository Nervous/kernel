#ifndef PCI_H_
# define PCI_H_

# include <kernel/list.h>
# include <kernel/types.h>

struct pci_driver_ops;

struct pci_dev {
	u8 bus;
	u8 device;
	u8 function;

	u16 vendor_id;
	u16 device_id;

	u32 bar[6];
	u32 bar_size[6];

	u8 intr_pin;
	u8 intr_line;

	struct list_node next_dev;

	const struct pci_driver_ops* ops;
};

struct pci_driver_ops {
	int (*attach)(struct pci_dev*);
	void (*detach)(struct pci_dev*);
};

int register_pci_driver(u16 vendor, u16 device,
			const struct pci_driver_ops* ops);
void pci_set_master(struct pci_dev* dev);

#endif /* !PCI_H_ */
