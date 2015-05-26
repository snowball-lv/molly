#include <molly.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	log("in init");

	int pid = exec("/shell.exe", -1, -1, -1);

	if (pid < 0) {
		log("unable to start shell");
		while(1);
	}

	log("shell started");
	while(1);

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
	while(1);
	
	return 1;
}