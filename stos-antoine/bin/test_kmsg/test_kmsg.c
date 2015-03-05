#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <kernel/klog_console.h>

#define FAIL(s)				\
	do {				\
		perror(s);		\
		exit(EXIT_FAILURE);	\
	} while(0)

#define FILL_VALUE	'a'
#define EXPECTED_LEN	(KLOG_CONSOLE_RING_BUFFER_SIZE - 1)


void fill_buffer(int fd)
{
	char* buf = malloc(KLOG_CONSOLE_RING_BUFFER_SIZE);
	if (!buf)
		FAIL("malloc failed");

	memset(buf, FILL_VALUE, KLOG_CONSOLE_RING_BUFFER_SIZE);

	if (write(fd, buf, KLOG_CONSOLE_RING_BUFFER_SIZE) !=
						KLOG_CONSOLE_RING_BUFFER_SIZE)
		FAIL("write failed");

	free(buf);
}

void read_filled_buffer(int fd)
{
	char* buf = malloc(KLOG_CONSOLE_RING_BUFFER_SIZE);
	if (!buf)
		FAIL("malloc failed");

	ssize_t res = read(fd, buf, KLOG_CONSOLE_RING_BUFFER_SIZE);
	if (res != EXPECTED_LEN)
		FAIL("read did not returned the right value");
	int i;
	for (i = 0; i < EXPECTED_LEN; i++)
		if (buf[i] != FILL_VALUE && (i != res - 1 || buf[i] != '\n'))
			FAIL("write did not fill correctly the buffer");

	free(buf);
}

void test_null_buffer(int fd)
{
	if (read(fd, NULL, 42) >= 0)
		FAIL("read did not returned the right value on NULL buffer");
	if (write(fd, NULL, 0) >= 0)
		FAIL("write did not returned the right value on NULL buffer");
}

void write_kmsg(int fd, const char* buf)
{
	if (write(fd, buf, strlen(buf)) < 0)
		FAIL("write failed");
}

int main(int argc, char** argv)
{
	int fd = open("/dev/kmsg", O_RDWR);
	if (fd < 0)
		FAIL("open /dev/kmsg failed");

	if (argc > 1) {
		write_kmsg(fd, argv[1]);
	} else {
		fill_buffer(fd);
		read_filled_buffer(fd);
		test_null_buffer(fd);
	}

	if (close(fd))
		FAIL("close failed");

	printf("OK\n");
	return 0;
}
