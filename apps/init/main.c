#include <molly.h>
#include <string.h>

int main(int argc, char **argv) {
	
	log("hello molly");
	
	int f = open("#console");

	if (f < 0) {
		log("couldn't open #console");
		while (1);
	}

/*
	char buff[1];
	while (read(f, buff, 1) > 0) {
		write(f, buff, 1);
	}
*/

	while(1);

	int rc = close(f);

	if (rc != 0) {
		log("couldn't close #console");
	}

	//exit will cause kernel panic
	while(1);
	
	return 1;
}