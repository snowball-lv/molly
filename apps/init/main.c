#include <molly.h>
#include <string.h>

int main(int argc, char **argv) {
	
	log("hello molly");
	
	int f = open("#console");

	//char *msg = "sent through write\n";
	//write(f, msg, strlen(msg));

	close(f);

	//exit will cause kernel panic
	while(1);
	
	return 1;
}