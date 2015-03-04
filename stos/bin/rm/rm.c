#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
		err(errno = EINVAL, "%s", "arguments");

	int i;
	int ret = 0;
	for (i = 1; i < argc && argv[i] != NULL; i += 1)
		if (unlink(argv[i]) < 0)
			err(errno, "%s", argv[i]);

	return ret;
}
