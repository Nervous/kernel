#ifndef KLOG_CONSOLE_H_
# define KLOG_CONSOLE_H_

# define KLOG_CONSOLE_RING_BUFFER_SIZE	512

size_t klog_console_read(char* buf, off_t offset, size_t count);
void klog_console_init(void);

#endif /* !KLOG_CONSOLE_H_ */
