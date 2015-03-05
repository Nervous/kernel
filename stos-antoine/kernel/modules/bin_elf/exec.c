/*
 * File: exec.c
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: Implements execve(2)
 */
#include <kernel/arch/current.h>
#include <kernel/elf.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/frame.h>
#include <kernel/kmalloc.h>
#include <kernel/limits.h>
#include <kernel/mem.h>
#include <kernel/module.h>
#include <kernel/page.h>
#include <kernel/stat.h>
#include <kernel/string.h>
#include <kernel/syscall.h>
#include <kernel/task.h>
#include <kernel/user.h>
#include <kernel/vfs.h>

static void copy_argv_envp(uword sp, uword* userstackptr,
			   char* argv[], char* envp[])
{
	/*
	 * For more information, see ELF i386 ABI section 3-28
	 */

	/*
	 * TODO: copy_user_buf should be used instead of memcpy, the strlen
	 * should also be "safe", copy_user_string? which does a strncpy?
	 */
	int argc;
	for (argc = 0; argv[argc]; argc++) {
		int len = strlen(argv[argc]);
		sp -= len + 1;
		*userstackptr -= len + 1;
		memcpy((void*)sp, argv[argc], len + 1);
		argv[argc] = (void*)*userstackptr;
	}

	int envc;
	for (envc = 0; envp && envp[envc]; envc++) {
		int len = strlen(envp[envc]);
		sp -= len + 1;
		*userstackptr -= len + 1;
		memcpy((void*)sp, envp[envc], len + 1);
		envp[envc] = (void*)*userstackptr;
	}

	/* Null auxiliary vector */
	sp -= sizeof(uword);
	*((uword*)sp) = 0;
	sp -= sizeof(uword);
	*((uword*)sp) = 0;

	/* 0 word between auxiliary vector and envp. */
	sp -= sizeof(uword);
	*((uword*)sp) = 0;

	sp -= envc * sizeof(uword);
	memcpy((void*)sp, envp, envc * sizeof(uword));

	/* 0 word between envp and argv. */
	sp -= sizeof(uword);
	*((uword*)sp) = 0;

	sp -= argc * sizeof(uword);
	memcpy((void*)sp, argv, argc * sizeof(uword));

	sp -= sizeof(uword);
	*((uword*)sp) = argc;

	*userstackptr -= (3 + envc + 2 + argc) * sizeof(uword);
}

static struct frame** build_stack(char* argv[], char* envp[], uword* userstackaddr)
{
	int stack_page_size = ARG_MAX / PAGE_SIZE;
	struct frame** res = kmalloc(sizeof(struct frame*) * stack_page_size);
	if (!res)
		return make_err_ptr(-ENOMEM);
	memset(res, 0, sizeof(struct frame*) * stack_page_size);

	int ret = -ENOMEM;
	for (int i = 0; i < stack_page_size; i++) {
		res[i] = alloc_frame();
		if (!res[i])
			goto err_alloc_frame;
	}
	void* stackaddr = alloc_pages(get_current()->mem->page_directory,
				      stack_page_size);
	if (!stackaddr)
		goto err_alloc_frame;

	ret = map_pages(get_current()->mem->page_directory, stackaddr,
			res, stack_page_size, P_KERNEL);
	if (ret < 0)
		goto err_alloc_frame;

	stackaddr += ARG_MAX;

	copy_argv_envp((uword)stackaddr, userstackaddr, argv, envp);

	unmap_pages(get_current()->mem->page_directory, res[0]->vaddr,
		    stack_page_size);

	return res;

err_alloc_frame:
	for (int i = 0; i < stack_page_size; i++)
		if (res[i])
			free_frame(res[i]);
	kfree(res, sizeof(struct frame*) * stack_page_size);

	return make_err_ptr(ret);
}

/*
 * Read all the segments into kernel memory.
 * Fills the p_paddr field with kmalloced adress.
 */
static int read_elf_segments(Elf_Phdr* phdr, int count, struct file* f)
{
	int ret = 0;

	/* Clear the p_addr in case of error. */
	for (int i = 0; i < count; i++)
		phdr[i].p_paddr = 0;

	for (int i = 0; i < count; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		ret = vfs_lseek(f, phdr[i].p_offset, SEEK_SET);
		if (ret < 0)
			goto err;

		ret = -ENOMEM;
		void* addr = kmalloc(phdr[i].p_filesz);
		if (!addr)
			goto err;

		phdr[i].p_paddr = (uword)addr;

		ret = vfs_read(f, addr, phdr[i].p_filesz);
		if (ret < 0)
			goto err;
	}

	return ret;

err:
	for (int i = 0; i < count && phdr[i].p_paddr; i++)
		kfree((void*)phdr[i].p_paddr, phdr[i].p_filesz);

	return ret;
}

static int add_elf_zones(Elf_Phdr* phdr, int count, struct task* t)
{
	int ret = 0;
	for (int i = 0; i < count; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		ret = add_mem_zone(get_current(), (void*)phdr[i].p_vaddr,
				   (void*)(phdr[i].p_vaddr + phdr[i].p_memsz));
		if (ret < 0) {
			release_mem(get_current()->mem);
			return ret;
		}

	}

	return ret;
}

static void copy_elf_segments(Elf_Phdr* phdr, int count)
{
	for (int i = 0; i < count; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		memcpy((void*)phdr[i].p_vaddr, (void*)phdr[i].p_paddr,
		       phdr[i].p_filesz);
		memset((void*)(phdr[i].p_vaddr + phdr[i].p_filesz), 0,
		       phdr[i].p_memsz - phdr[i].p_filesz);

		kfree((void*)phdr[i].p_paddr, phdr[i].p_filesz);
	}
}

long __syscall sys_execve(char* path, char* argv[], char* envp[])
{
	if (!argv)
		return -EFAULT;

	char* kpath = kmalloc(PATH_MAX);
	if (!kpath)
		return -ENOMEM;
	char* kpath_sav = kpath;

	int ret = copy_user_buf(kpath, path, PATH_MAX);
	if (ret < 0) {
		kfree(kpath, PATH_MAX);
		return ret;
	}

	struct inode* ino = path_lookup(&kpath);
	kfree(kpath_sav, PATH_MAX);
	if (is_err_ptr(ino))
		return get_err_ptr(ino);

	if (!has_access(ino, S_IXUSR))
		return -EACCES;

	struct file* f = vfs_open(ino, O_RDONLY, 0);
	if (is_err_ptr(f))
		return get_err_ptr(f);

	Elf_Ehdr hdr;
	ret = vfs_read(f, &hdr, sizeof(Elf_Ehdr));
	if (ret < 0)
		goto fail_read_file;

	ret = -ENOEXEC;
	if (!IS_ELF(hdr))
		goto fail_read_file;

	ret = -EINVAL;
	if (hdr.e_machine != EM_MACHINE || hdr.e_type != ET_EXEC)
		goto fail_read_file;

	vfs_lseek(f, hdr.e_phoff, SEEK_SET);

	ret = -ENOMEM;
	Elf_Phdr* phdr = kmalloc(sizeof(Elf_Phdr) * hdr.e_phnum);
	if (!phdr)
		goto fail_read_file;

	ret = vfs_read(f, phdr, sizeof(Elf_Phdr) * hdr.e_phnum);
	if (ret < 0)
		goto fail_read_phdr;

	uword old_userstack = *get_user_stack(get_current()->regs);
	void* userstack = (void*)0xB0000000;
	*get_user_stack(get_current()->regs) = (uword)userstack;

	struct frame** stack = build_stack(argv, envp, get_user_stack(get_current()->regs));
	if (is_err_ptr(stack)) {
		ret = get_err_ptr(stack);
		goto fail_build_stack;
	}

	/*
	 * First, read the whole binary into kernel space so that we can return
	 * to the unchanged process if something bad happen.
	 */
	ret = read_elf_segments(phdr, hdr.e_phnum, f);
	if (ret < 0)
		goto fail_read_elf_segments;

	ret = -ENOMEM;
	struct mem* new_mem = kmalloc(sizeof(struct mem));
	if (!new_mem)
		goto fail_alloc_new_mem;

	struct mem* old_mem = get_current()->mem;
	new_mem->page_directory = old_mem->page_directory;
	list_init(&new_mem->zones);
	rw_spinlock_init(&new_mem->mem_lock);
	get_current()->mem = new_mem;

	ret = add_elf_zones(phdr, hdr.e_phnum, get_current());
	if (ret < 0)
		goto fail_add_elf_zones;

	ret = add_mem_zone(get_current(), userstack - ARG_MAX, userstack);
	if (ret < 0)
		goto fail_add_stack_zone;

	/*
	 * Second, release the old process memory
	 */
	release_mem(old_mem);
	kfree(old_mem, sizeof(struct mem));

	/*
	 * Finally, copy the process into its adress space.
	 */
	copy_elf_segments(phdr, hdr.e_phnum);

	ret = map_pages(get_current()->mem->page_directory, userstack - ARG_MAX,
			stack, ARG_MAX / PAGE_SIZE, P_USER_RW);
	if (ret < 0)
		goto fail_map_stack;

	for (u32 i = 0; i < ARG_MAX / PAGE_SIZE; i++)
		refcnt_dec(&stack[i]->cnt);

	kfree(stack, sizeof(struct frame*) * ARG_MAX / PAGE_SIZE);

	prepare_new_task(get_current(), (void*)hdr.e_entry, USER_TASK);

	kfree(phdr, sizeof(Elf_Phdr) * hdr.e_phnum);
	vfs_close(f);

	/*
	 * Close the fds that should be closed on exec.
	 */
	spinlock_lock(&get_current()->fds->files_lock);
	for (int i = 0; i < get_current()->fds->max_fds; i++)
		if (get_current()->fds->files[i]
		    && get_current()->fds->files[i]->f_flags & FD_CLOEXEC) {
			vfs_close(get_current()->fds->files[i]);
			get_current()->fds->files[i] = NULL;
		}
	spinlock_unlock(&get_current()->fds->files_lock);

	/*
	 * The new task will automagically been executed, except if
	 * sys_execve is called in kernel land which should only happen
	 * on the first task.
	 */
	return 0;

fail_map_stack:
	panic("Cannot map the new stack.");

fail_add_stack_zone:
	release_mem(get_current()->mem);
fail_add_elf_zones:
	kfree(new_mem, sizeof(struct mem));
	get_current()->mem = old_mem;
fail_alloc_new_mem:
	for (int i = 0; i < hdr.e_phnum; i++)
		if (phdr[i].p_type == PT_LOAD)
			kfree((void*)phdr[i].p_paddr, phdr[i].p_filesz);
fail_read_elf_segments:
	for (unsigned int i = 0; i < ARG_MAX / PAGE_SIZE; i++)
		free_frame(stack[i]);
	kfree(stack, sizeof(struct frame*) * ARG_MAX / PAGE_SIZE);
fail_build_stack:
	*get_user_stack(get_current()->regs) = old_userstack;
fail_read_phdr:
	kfree(phdr, sizeof(Elf_Phdr) * hdr.e_phnum);
fail_read_file:
	vfs_close(f);

	return ret;
}
EXPORT_SYMBOL(sys_execve);

static void __init_once init(void)
{
	register_syscall(sys_execve, SYS_EXECVE);
}
MODINFO {
	module_name("bin_elf"),
	module_init_once(init),
	module_type(M_BIN),
	module_deps(M_VFS | M_TASK | M_KMALLOC | M_PAGINATION
		    | M_PAGE_ALLOCATOR | M_SYSCALL)
};
