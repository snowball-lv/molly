#include <molly.h>
#include <stdio.h>
#include <string.h>

#define CONSOLE_BUFFER_SIZE  (4096 * 2)

static char buffer[CONSOLE_BUFFER_SIZE];

int main(int argc, char **argv) {

	printf("---- MOLLY SHELL ----\n");

	while (gets(buffer) != 0) {
		
	}

	return 0;
}