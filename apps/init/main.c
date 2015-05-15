#include <molly.h>
#include <string.h>

int main(int argc, char **argv) {
	
	log("hello molly");
	
	int f = open("#console");

	char buff[1];
	while (read(f, buff, 1) > 0) {
		log("user read");
		//write(f, buff, 1);
	}

	close(f);

	//exit will cause kernel panic
	while(1);
	
	return 1;
}