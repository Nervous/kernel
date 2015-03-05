#ifndef FD_HANDLER_H_
# define FD_HANDLER_H_

# include <kernel/vfs.h>

int file_install(struct file* file, uword min);

/* Assume that the fd is correct */
struct file* file_get(int fd);
void file_release(int fd);
int is_valid_fd(int fd);

#endif /* !FD_HANDLER_H_ */
