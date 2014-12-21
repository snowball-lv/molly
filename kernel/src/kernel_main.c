#include <types.h>
#include <console.h>

word kernel_main() {

	clear();
	println("executing kernel...");
	
	for (word i = 0; i < 30; i++) {
		printWord(i); println("");
	}
	
	return 0xbabecafe;
}