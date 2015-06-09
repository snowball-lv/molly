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
		fileno(console),
		fileno(console),
		fileno(console));

	if (pid < 0) 
		log("unable to start shell");
	else
		log("shell started");

	//wait for shell to exit
	waitpid(pid);
	
	log("shell exited");

	return 0;
}