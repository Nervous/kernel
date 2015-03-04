#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int open_default_fd(void)
{
	int fd = open("/dev/com/2", O_RDWR);
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	//close(fd);

	return 0;
}

int main(int argc, char** argv)
{
	if (mkdir("/dev", 0777) < 0 && errno != EEXIST)
		return 1;

	if (mount(NULL, "/dev", "devfs") < 0)
		return 1;

	if (open_default_fd() < 0)
		return 1;

	if (!fork()) {
		putenv("HOME=/");
		putenv("PWD=/");
		char* argp[] = { "/mnt/bin/dash", NULL };
		execvp("/mnt/bin/dash", argp);
		return 1;
	}

	while (1) {
		int status;
		printf("%d just died.\n", wait(&status));
	}
}
