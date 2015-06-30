#include <molly.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	size_t cwd_len = getcwd(0);
	char *cwd = malloc(cwd_len + 1);
	getcwd(cwd);

	printf("Contents of: [%s]\n", cwd);

	dirent_t e;
	int dir = open(cwd);
	free(cwd);

	while (readdir(dir, &e) != 0) {
		printf("%s\n", e.name);
	}
	close(cwd);

	return 0;
}