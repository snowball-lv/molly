#include <molly.h>
#include <string.h>
#include <stdlib.h>

static dirent_t e;

int main(int argc, char **argv) {
	
	log("hello molly");

	int root = open("/");

	if (root < 0) {
		log("unable to open root");
		while(1);
	}

	/*
	int term = open("#console");

	if (term < 0) {
		log("failed to open console");
		while(1);
	}
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