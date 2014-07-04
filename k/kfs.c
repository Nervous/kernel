#include "kfs.h"
#include "kstd.h"
#include "string.h"

static t_uint32
kfs_checksum (const void *data, size_t sz) {
    const t_uint8 *ptr = data;
    t_uint32 a = 1, b = 0;

    for (; sz; ++ptr, --sz) {
        a = (a + *ptr) % ADLER32_MOD;
        b = (b + a) % ADLER32_MOD;
    }
    return (b << 16) | a;
}


struct kfs_inode* fd_table[256];
int seek_table[256];
multiboot_info_t* info = NULL;
void *entry;

static int check_magic(struct kfs_superblock sb)
{
    return sb.magic == KFS_MAGIC;
}

static struct kfs_inode *get_inode(const char *name)
{
    void* first_inode = entry + KFS_BLK_SZ;
    void *act_inode = first_inode;
    t_uint32 act_index = 0;
    while (strcmp(name, ((struct kfs_inode *) act_inode)->filename) != 0)
    {
        act_index = ((struct kfs_inode *) act_inode)->next_idx;
        act_inode = entry + (act_index * KFS_BLK_SZ);
        if (act_inode == first_inode)
            break;
    }
    if (strcmp(name, ((struct kfs_inode *) act_inode)->filename) != 0)
        return NULL;
    return (struct kfs_inode *) act_inode;
}

int init_kfs(multiboot_info_t* inf)
{
    for (int i = 0; i < 256; ++i)
    {
        fd_table[i] = NULL;
        seek_table[i] = 0;
    }
    info = inf;
    entry = (void *)((module_t*)info->mods_addr)[0].mod_start;
    struct kfs_superblock *sb = (struct kfs_superblock *) entry;
    if (check_magic(*sb) != 1)
        return 1;
    if (kfs_checksum((void *) sb, 52) != sb->cksum) // 52 bytes is the size ofthe first 6 fields of sb
        return 2;
    memset(fd_table, 0, sizeof(struct kfs_block) * sb->files_cnt);
    return 0;
}

int open(const char *pathname, int flags)
{
    if (flags != O_RDONLY)
        return -1;
    struct kfs_inode *in = get_inode(pathname);
    if (in == NULL)
        return -1;
    int i = 0;
    for (i = 0; fd_table[i] != NULL && i < 256; ++i)
        continue;
    fd_table[i] = in;
    return i;
}

ssize_t read(int fd, void *buf, size_t count)
{
    char* buffer = (char *) buf;
    struct kfs_inode* in = fd_table[fd];
    buffer[0] = 'a';
    if (in == NULL)
        return -1;
    if (in->i_blk_cnt == 0 && in->d_blk_cnt == 0) // empty file
        return 0;
    t_uint32 copied = 0;
    size_t act_i_block = 0;
    size_t act_d_block = 0;
    struct kfs_i_block_index* i_block = NULL;
    struct kfs_block* d_block;
    size_t starting_pos = seek_table[fd];

    if (in->i_blk_cnt == 0)
    {
        while (act_d_block < in->d_blk_cnt)
        {
            d_block = entry + (in->d_blks[act_d_block++] * KFS_BLK_SZ);
            
            while (copied < starting_pos + count && copied < KFS_BLK_DATA_SZ
                   && copied < in->file_sz)
            {
                if (copied < starting_pos)
                {
                    ++copied;
                    continue;
                }
                int act = copied - starting_pos;
                buffer[act] = d_block->data[copied++];
            }

            if (copied == starting_pos + count || copied == in->file_sz)
                break;
        }
    }
    else
    {

        while (act_i_block < in->i_blk_cnt)
        {
            i_block = entry + (in->i_blks[act_i_block++] * KFS_BLK_SZ);

            while (act_d_block < in->d_blk_cnt)
            {
                d_block = entry + (i_block->blks[act_d_block++] * KFS_BLK_SZ);

                while (copied < starting_pos + count && copied < KFS_BLK_DATA_SZ
                       && copied < in->file_sz)
                {
                    if (copied < starting_pos)
                    {
                        ++copied;
                        continue;
                    }
                    int act = copied - starting_pos;
                    buffer[act] = d_block->data[copied++];
                }

                if (copied == starting_pos + count || copied == in->file_sz)
                    break;
            }

            if (copied == count || copied == in->file_sz)
                break;
        }
    }
    seek_table[fd] = copied;
    return copied - starting_pos;
}

off_t seek(int fd, off_t offset, int whence)
{
    if (whence == SEEK_SET)
        seek_table[fd] = offset;
    else if (whence == SEEK_CUR)
        seek_table[fd] += offset;
    else if (whence == SEEK_END)
        seek_table[fd] = fd_table[fd]->file_sz + offset;

    return 0;
}

int close(int fd)
{
    if (fd >= 256 || fd_table[fd] == NULL)
        return -1;
    fd_table[fd] = NULL;
    return 0;
}
