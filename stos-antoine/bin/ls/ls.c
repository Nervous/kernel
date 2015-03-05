#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <err.h>

static int print_dir(char* dirname)
{
	DIR* cur = opendir(dirname);
	if (cur == NULL)
		return -1;

	printf("%s: ", dirname);

	struct dirent* e = readdir(cur);
	for (; e != NULL; e = readdir(cur)) {
		/* Avoid trailing slash */
		if (dirname == NULL)
			printf(" ");
		else
			dirname = NULL;

		printf("%s", e->d_name);
	}

	closedir(cur);
	printf("\n");
	return 0;
}

struct file {
	char* name;
	struct file* next;
};

static struct file* tail = NULL;
static struct file* head = NULL;

static void add_file(char* fname)
{
	if (tail != NULL) {
		tail->next = malloc(sizeof (struct file));
		tail = tail->next;
	}
	else
		tail = malloc(sizeof (struct file));

	if (head == NULL)
		head = tail;
	tail->next = NULL;
	tail->name = fname;
}


void add_current_dir(void)
{
	struct stat buf;
	if (stat(".", &buf) < 0)
		err(errno, "%s", ".");
	else if ((buf.st_mode & S_IFDIR) && print_dir(".") < 0)
		err(errno, "%s", ".");
	else if ((buf.st_mode & S_IFDIR) == 0)
		add_file(".");
}

int main(int argc, char* argv[])
{
	int i;
	int ret = 0;

	if (argc == 1)
		add_current_dir();

	for (i = 1; i < argc && argv[i] != NULL; i += 1) {
		struct stat buf;
		if (stat(argv[i], &buf) < 0)
			err(errno, "%s", argv[i]);
		else if ((buf.st_mode & S_IFDIR) && print_dir(argv[i]) < 0)
			err(errno, "%s", argv[i]);
		else if ((buf.st_mode & S_IFDIR) == 0)
			add_file(argv[i]);
	}

	if (head != NULL) {
		for (; head != NULL; head = head->next) {
			printf("%s", head->name);

			/* Avoid trailing slash */
			if (head->next != NULL)
				printf(" ");
		}

		printf("\n");
	}

	return ret;
}
