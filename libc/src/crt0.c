#include <stdlib.h>
#include <stdio.h>
#include <molly.h>


static void init_libc() {
	
	stdin = malloc(sizeof(FILE));
	stdin->fd = 0;
	
	stdout = malloc(sizeof(FILE));
	stdout->fd = 1;
	
	stderr = malloc(sizeof(FILE));
	stderr->fd = 2;
}


static void run_constructors() {

}


//user process entry
int main(int argc, char **argv);


//os entry into process, called by loader
void _molly_entry(int argc, char **argv) {

	//initialize standard library
	init_libc();

	//call global constructors
	run_constructors();

	int rc = main(argc, argv);

	exit(rc);
}