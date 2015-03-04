/*
 * To compile : gcc -std=c99 stos_shell.c -o stos_shell
 * To use : stos_shell /dev/pts/3
 */


#include <err.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#define STDIN_INPUT	0
#define STOS_OUTPUT	1

static void read_and_output(int fdinput, int fdoutput)
{
	char buf[512];

	int n;
	while ((n = read(fdinput, buf, 512)) == 512)
		write(fdoutput, buf, 512);
	if (n < 0)
		exit(1);
	else if (n == 0)
		exit(0);
	else
		write(fdoutput, buf, n);
}

static void read_stos_output(struct pollfd* pollfd)
{
	read_and_output(pollfd[STOS_OUTPUT].fd, STDOUT_FILENO);
}

static void read_stdin_input(struct pollfd* pollfd)
{
	read_and_output(STDIN_FILENO, pollfd[STOS_OUTPUT].fd);
}

static void (*const fd_handlers[2])(struct pollfd* pollfd) = {
	read_stdin_input, read_stos_output
};

int main(int argc, char** argv)
{
	if (argc < 2)
		errx(1, "Please provide the pty of stos serial");

	int fd = open(argv[1], O_RDWR);
	if (fd < 0)
		errx(1, "Please provide the pty of stos serial");

	struct pollfd fds[2] = {
		{ .fd = STDIN_FILENO, .events = POLLIN },
		{ .fd = fd, .events = POLLIN }
	};

	while (1) {
		int n = poll(fds, 2, -1);
		if (n < 0)
			errx(1, "Fail poll\n");

		for (int i = 0; n && i < 2; i++) {
			if (fds[i].revents != 0) {
				n--;
				fd_handlers[i](fds);
			}
		}
	}
}
