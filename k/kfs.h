#ifndef KFS_H
# define KFS_H


# include "kstd.h"
# include "multiboot.h"

#define KFS_MAGIC 0xd35f9caa

#define KFS_MIN_BLK_SZ 512

#ifdef KFS_BLK_SZ
# if (KFS_BLK_SZ < KFS_MIN_BLK_SZ)
#   define KFS_BLK_SZ KFS_MIN_BLK_SZ
# endif
#else
# define KFS_BLK_SZ 4096
#endif

#define KFS_BLK_DATA_SZ (KFS_BLK_SZ - 3 * 4)

#ifndef KFS_DIRECT_BLK
# define KFS_DIRECT_BLK 10
#endif

#ifndef KFS_INDIRECT_BLK
# define KFS_INDIRECT_BLK 16
#endif

#ifndef KFS_INDIRECT_BLK_CNT
# define KFS_INDIRECT_BLK_CNT 16
#endif

#ifndef KFS_FNAME_SZ
# define KFS_FNAME_SZ 32
#endif

#ifndef KFS_NAME_SZ
# define KFS_NAME_SZ 32
#endif

#define kfswrite(Buf, Sz, Nm, S) \
  do { \
    if (!fwrite (Buf, Sz, Nm, S)) { \
      fprintf (stderr, "error writing to file\n"); \
      fclose (S); \
      exit (EXIT_FAILURE); \
    } \
  } \
  while (0)

#define OPT_VERBOSE (1 << 0)

struct kfs_block {
  t_uint32 idx;
  t_uint32 usage;
  t_uint32 cksum;
  char data[KFS_BLK_DATA_SZ];
} __attribute__((packed));

struct kfs_i_block_index {
  t_uint32 idx;
  t_uint32 blk_cnt;
  t_uint32 blks[KFS_INDIRECT_BLK_CNT];
  t_uint32 cksum;
} __attribute__((packed));

struct kfs_inode {
  t_uint32 inumber;
  char filename[KFS_FNAME_SZ];
  t_uint32 file_sz;
  t_uint32 idx;
  t_uint32 blk_cnt;
  t_uint32 next_idx;
  t_uint32 d_blk_cnt;
  t_uint32 i_blk_cnt;
  t_uint32 d_blks[KFS_DIRECT_BLK];
  t_uint32 i_blks[KFS_INDIRECT_BLK];
  t_uint32 cksum;
} __attribute__((packed));

struct kfs_superblock {
  t_uint32 magic;
  char name[KFS_NAME_SZ];
  t_uint32 tstamp;
  t_uint32 blk_cnt;
  t_uint32 files_cnt;
  t_uint32 inode_idx;
  t_uint32 cksum;
} __attribute__((packed));

#define ADLER32_MOD 65521
int init_kfs();

#endif

int init_kfs(multiboot_info_t* inf);
