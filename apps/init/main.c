#include <molly.h>
#include <string.h>

int main(int argc, char **argv) {
	
	log("hello molly");
	
	int f = open("#console");
	char *msg = "sent through write";
	write(f, msg, strlen(msg));
	close(f);
	
	return 1;
}