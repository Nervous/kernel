#include <kernel/cpio.h>
#include <kernel/string.h>
#include <kernel/vfs.h>
#include <kernel/fcntl.h>
#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/panic.h>

static unsigned int cpio_hexatoi(const char* str)
{
	unsigned int res = 0;
	for (const char* c = str; c < str + 8; c++) {
		res *= 16;
		if ('0' <= *c && *c <= '9')
			res += *c - '0';
		else if ('a' <= *c && *c <= 'f')
			res += *c - 'a' + 10;
		else if ('A' <= *c && *c <= 'F')
			res += *c - 'A' + 10;
		else
			panic("Invalid hexadecimal value in initramfs' cpio");
	}

	return res;
}

static struct cpio_entry cpio_load_entry(struct cpio_newc_header* header)
{
	if (strncmp("070701", header->c_magic, 6))
		panic("Invalid magic cpio entry number in initramfs");

	struct cpio_entry entry;

	entry.namesize = cpio_hexatoi(header->c_namesize);

	entry.name = kmalloc(entry.namesize + 1);
	strncpy(entry.name, (char*)(header + 1), entry.namesize);
	entry.name[entry.namesize] = '\0';
	int name_padding = (4 - (sizeof (*header) + entry.namesize) % 4) % 4;

	entry.filesize = cpio_hexatoi(header->c_filesize);
	entry.file = (u8*)(header + 1) + entry.namesize + name_padding;
	int file_padding = (4 - (entry.filesize % 4)) % 4;

	entry.mode = cpio_hexatoi(header->c_mode);

	entry.next_header = (struct cpio_newc_header*)(entry.file
						       + entry.filesize
						       + file_padding);

	return entry;
}

static inline void cpio_free_entry(struct cpio_entry entry)
{
	kfree(entry.name, entry.namesize + 1);
}

static char* cpio_basename(struct cpio_entry entry)
{
	size_t i;
	for (i = entry.namesize - 1; i > 0; i--)
		if (entry.name[i] == '/')
			return entry.name + i + 1;
	return entry.name;
}

static char* cpio_make_parent_path(struct cpio_entry entry, char* path)
{
	char* res = (char*)kmalloc(entry.namesize + strlen(path) + 2);

	size_t last_slash = 0;
	for (size_t i = 0; i < entry.namesize - 1; i++)
		if (entry.name[i] == '/')
			last_slash = i;

	size_t i = 0;
	for (; path[i] != 0; i++)
		res[i] = path[i];

	if (last_slash)
		res[i++] = '/';

	size_t j = 0;
	for (; j < last_slash; j++)
		res[i + j] = entry.name[j];
	res[i + j] = '\0';

	return res;
}

static void cpio_write_entry(struct cpio_entry entry, char* path)
{
	char* parent_path = cpio_make_parent_path(entry, path);
	char* backup_ptr = parent_path;
	struct inode* parent_inode = path_lookup(&parent_path);
	kfree(backup_ptr, entry.namesize + strlen(path) + 2);
	if (is_err_ptr(parent_inode)) {
	    panic("Invalid cpio in initramfs: "
		  "please define directories before their content");
	}

	struct inode* entry_inode;
	mode_t file_mode = 0777;

	if (entry.mode & CPIO_ISDIR) {
		entry_inode = vfs_create_file(parent_inode,
					      cpio_basename(entry),
					      O_DIRECTORY | O_CREAT,
					      file_mode);
		if (is_err_ptr(entry_inode))
			panic("Could not create file while unpacking initramfs");
		return;
	}
	else
		entry_inode = vfs_create_file(parent_inode,
					      cpio_basename(entry),
					      O_CREAT,
					      file_mode);

	if (is_err_ptr(entry_inode))
		panic("Could not create file while unpacking initramfs");

	struct file* entry_file = vfs_open(entry_inode, O_WRONLY, 0);
	if (is_err_ptr(entry_file))
		panic("Could not open file while unpacking initramfs");

	ssize_t ret = vfs_write(entry_file, entry.file, entry.filesize);
	if (ret < 0)
		panic("Could not write to file while unpacking initramfs");

	return;
}

void cpio_unpack_archive(void* archive_data, char* unpack_path)
{
	struct cpio_entry entry = cpio_load_entry(archive_data);
	while (1) {
		entry = cpio_load_entry(entry.next_header);
		if (entry.namesize >= 10 && !strcmp("TRAILER!!!", entry.name)) {
			cpio_free_entry(entry);
			break;
		}
		cpio_write_entry(entry, unpack_path);
		cpio_free_entry(entry);
	}
}
