#include <unistd.h>
#include <fcntl.h>

#include <errno.h>
#include <err.h>

#define BUFLEN 0x200

int main(int argc, char* argv[])
{
	if (argc < 2)
		return 1;

	int i;
	for (i = 1; i < argc; i += 1) {
		int fd = open(argv[i], O_RDONLY);
		if (fd < 0)
			err(errno, "%s", argv[i]);

		int len;
		char buf[BUFLEN];
		while ((len = read(fd, buf, BUFLEN)) != 0) {
			if (len < 0)
				err(errno, "%s", argv[i]);
			write(STDOUT_FILENO, buf, len);
		}

		close(fd);
	}

	return 0;
}
