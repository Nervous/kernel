#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <err.h>

int main(int argc, char* argv[])
{
	mode_t mode = 0777;

	if (argc < 2)
		err(errno = EINVAL, "%s", "arguments");

	int i;
	int ret = 0;
	for (i = 1; i < argc; i += 1) {
		if (mkdir(argv[i], mode) < 0)
			err(errno, "%s", argv[i]);
	}

	return ret;
}
