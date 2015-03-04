/*
 * File: klog_console.c
 *
 * Description: ring buffer used as a console in klog
*/

# include <kernel/klog.h>
# include <kernel/klog_console.h>
# include <kernel/module.h>
# include <kernel/ring_buffer.h>

static struct ring_buffer ring_buffer;

static int klog_console_write(char* buf, size_t len)
{
	return ring_buffer_write(&ring_buffer, (u8*)buf, len);
}

size_t klog_console_read(char* buf, off_t offset, size_t count)
{
	return ring_buffer_read(&ring_buffer, offset, (u8*)buf, count);
}
EXPORT_SYMBOL(klog_console_read);

static struct console_ops klog_console = {
	.name = "klog",
	.write = klog_console_write
};

void klog_console_init(void)
{
	if (!ring_buffer_init(&ring_buffer, KLOG_CONSOLE_RING_BUFFER_SIZE))
		add_console(&klog_console);
}
