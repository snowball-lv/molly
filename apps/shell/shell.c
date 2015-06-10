#include <molly.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CONSOLE_BUFFER_SIZE  (4096 * 2)

char buffer[CONSOLE_BUFFER_SIZE];

int main(int argc, char **argv) {

	printf("---- MOLLY SHELL ----\n");

	memset(buffer, 0, CONSOLE_BUFFER_SIZE);

	size_t cwd_len = getcwd(0);
	char *cwd = malloc(cwd_len + 1);
	getcwd(cwd);

	printf("%s>", cwd);

	while (gets(buffer) != 0) {

		//process line

		printf("%s>", cwd);
	}

	return 0;
}

