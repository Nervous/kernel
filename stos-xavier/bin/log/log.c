#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int i;
	int klog = open("/dev/klog/1", O_RDWR);

	for (i = 1; i < argc; ++i) {
		write(klog, argv[i], strlen(argv[i]));
	}

	close(klog);

	return 0;
}
