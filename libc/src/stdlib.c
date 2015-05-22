#include <stdlib.h>
#include <molly.h>

void *malloc(size_t size) {

	if (size == 0)
		return 0;

	return sbrk(size);
}


void free(void *ptr) {
	log("free");
	return;
}