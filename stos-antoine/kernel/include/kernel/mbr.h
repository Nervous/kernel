/*
 * File: mbr.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 */
#ifndef MBR_H_
# define MBR_H_

# include <kernel/stos.h>

struct mbr_partition {
	u8 status;
	u8 chs_start[3];
	u8 type;
	u8 chs_end[3];
	u32 start_lba;
	u32 sectors_count;
} __packed;

#endif /* !MBR_H_ */
