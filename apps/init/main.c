#include <molly.h>
#include <string.h>

int main(int argc, char **argv) {
	
	log("hello molly");

	int term = open("#console");

	if (term < 0) {
		log("failed to open console");
		while(1);
	}

	char *buff[16];
	read(term, buff, 16);

	log("read returned");

	//exit will cause kernel panic
	while(1);
	
	return 1;
}