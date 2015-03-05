#ifndef USER_H_
# define USER_H_

/*
 * Copy memory from user space to kernel space.
 * Return a negative number if it fails.
 */
int copy_user_buf(void* kbuf, const void* ubuf, size_t n);

int strncopy_user_buf(void* kbuf, const void* ubuf, size_t n);

/*
 * Write kernel memory into user space.
 * Return a negative number if it fails.
 */
int write_user_buf(void* ubuf, const void* kbuf, size_t n);

#endif /* !USER_H_ */
