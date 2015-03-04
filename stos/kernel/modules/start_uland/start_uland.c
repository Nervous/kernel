/*
 * File: start_uland.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Stos init task.
 */

#include <kernel/arch/current.h>
#include <kernel/arch/switch_to.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/klog.h>
#include <kernel/module.h>
#include <kernel/panic.h>
#include <kernel/sched.h>
#include <kernel/syscall.h>
#include <kernel/task.h>
#include <kernel/vfs.h>
#include <kernel/cpio.h>
#include <boot/interface.h>

static void init_main(void)
{
	if (vfs_mount(NULL, get_current()->fs->root, "ramfs") < 0)
		panic("Cannot mount ramfs to /");
	get_current()->fs->pwd = get_current()->fs->root;

	char* path = "dev";
	struct inode* devino = vfs_create_file(get_current()->fs->root, path,
					       O_DIRECTORY | O_CREAT, 0777);
	if (is_err_ptr(devino))
		panic("Cannot create /dev");

	if (vfs_mount(NULL, devino, "devfs") < 0)
		panic("Cannot mount devfs to /dev");

	path = "mnt";
	struct inode* mntino = vfs_create_file(get_current()->fs->root, path,
					       O_DIRECTORY | O_CREAT, 0777);

	struct boot_interface* boot = get_boot_interface();
	if (boot->initramfs)
		cpio_unpack_archive(boot->initramfs, "/mnt");
	else {
		path = "/dev/hd/1";
		struct inode* blk = path_lookup(&path);
		if (is_err_ptr(blk))
			panic("Cannot find /dev/hd/1");
		if (vfs_mount(blk->i_part, mntino, "ext2fs") < 0)
			panic("Cannot mount /mnt");
	}

	klog("Launching init\n");

	char* argv[] = { "/mnt/bin/init", NULL };
	char* envp[] = { "PATH=/mnt/bin", NULL };
	if (sys_execve("/mnt/bin/init", argv, envp) >= 0)
		switch_to_task(get_current(), get_current());

	panic("Could not launch /mnt/bin/init!");
}

void start_uland(void)
{
	struct task* init_task = clone_task(get_current(), CLONE_FORK);
	prepare_new_task(init_task, init_main, KERNEL_TASK);
	init_task->sched_attr->prio = USER_PRIO;

	init_task->state = TASK_WAITING;
	enqueue_task(init_task);

	idle();
}
EXPORT_SYMBOL(start_uland);

MODINFO {
	module_name("start_uland"),
	module_deps(M_TASK | M_VFS),
	module_type(M_START_ULAND),
};
