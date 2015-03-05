#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	while (1) {
		char buf[64];
		int i = 0;

		while (read(STDIN_FILENO, &buf[i], 1) >= 0 && buf[i] != '\r') {
			write(STDOUT_FILENO, &buf[i], 1);
			i++;
		}

		buf[i + 1] = '\n';
		write(STDOUT_FILENO, &buf[i], 2);
	}
}
