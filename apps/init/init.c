#include <molly.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int thread() {
	log("in init thread");
	exit_t();
	return 0;
}

int main(int argc, char **argv) {

	log("in init");

	FILE *console = fopen("#console", "todo");

	int pid = exec(
		"/shell.exe",
		-1,
		fileno(console),
		fileno(console));

	if (pid < 0) {
		log("unable to start shell");
		while(1);
	}

	log("shell started");

	//mkt(thread);

	/*
	
	int term = open("#console");

	if (term < 0) {
		log("failed to open console");
		while(1);
	}

	char *welcome = "--- molly shell ---";
	write(term, welcome, strlen(welcome));

	size_t plen = getcwd(0);
	char *pbuff = malloc(plen);
	getcwd(pbuff);

	char *inbuff = malloc(64);

	while (1) {
		write(term, pbuff, plen);
		write(term, ">", 1);
		read(term, inbuff, 64);
	}

	log("term loop exit");

	*/

	//exit will cause kernel panic
	
	return 0;
}