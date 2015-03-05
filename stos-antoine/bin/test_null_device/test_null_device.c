#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void)
{
	char buf[10] = "";

	int fd = open("/dev/null", O_RDWR);
	if (fd < 0) {
		perror("open failed");
		return 1;
	}
	if (write(fd, "toto\n", 5) != 5) {
		perror("write failed");
		return 1;
	}
	if (lseek(fd, 2, SEEK_SET)) {
		perror("lseek failed");
		return 1;
	}
	if (read(fd, buf, 10) || strcmp(buf, "")) {
		perror("read failed");
		return 1;
	}
	if (close(fd)) {
		perror("close failed");
		return 1;
	}

	printf("OK\n");
	return 0;
}
