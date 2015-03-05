/*
 * File: device.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Device manager interface.
 */
#ifndef DEVICE_H_
# define DEVICE_H_

# include <kernel/list.h>
# include <kernel/vfs.h>

struct major;

/* The register_device function takes:
 *	- parent inode (the directory containing the major device) in the devfs
 *	  (otherwise will return -EINVAL)
 *	- the name of the device (hd, kb, klog, ..)
 *	- the file_ops structure of operations for this specific device
 *	- a void pointer to a structure useful for the file operations
 *
 * A single device registered via this function is just an inode in the vfs. It
 * is just a single device with its own file operations.
 *
 * Examples may include:
 *	- /dev/null
 *	- /dev/zero
 *	- /dev/ttyctrl
 *	...
 */
struct inode* register_device(struct inode* parent, char *name,
	const struct file_ops* fops, void* device_data);

/* The register_major function takes:
 *	- parent inode (the directory containing the major device) in the devfs
 *	  (otherwise will return -EINVAL)
 *	- the name of the device (hd, kb, klog, ..)
 *	- the file_ops structure of operations for this specific device
 *	- a void pointer to a structure useful for the file operations
 *
 * It is the implementation of a driver, it contains file operations and every
 * minor that implements this major contains the same file operations and is
 * contained in the matching folder in the devfs.
 *
 * Examples may include:
 *	- /dev/com/{1,2,3...}
 *	- /dev/hd/{1,2,3,4...}
 *	...
 */
struct major* register_major(struct inode* parent, char *name,
	const struct file_ops* fops, void* device_data);

/* The register_minor function takes:
 *	- a major device pointer, this is the major device containing the file
 *	  operation functions to use later on
 *	- a void pointer to a structure useful for the file operations specific
 *	  to this minor device
 */
int register_minor(struct major* major, void* device_data);

/*
 * Self-explanatory
 */
int unregister_device(struct inode* device);

/*
 * Self-explanatory
 */
int unregister_major(struct major* device);

/*
 * Self-explanatory
 */
int unregister_minor(struct major* major, int idx);

#endif /* !DEVICE_H_ */
