#include <stdio.h>
#include <stdlib.h>
#include <molly.h>
#include <limits.h>


FILE *fopen(const char *filename, const char *mode) {

	int fd = open((char *)filename);

	if (fd < 0)
		return 0;

	FILE *f = malloc(sizeof(FILE));

	f->fd = fd;

	return f;
}


int fclose(FILE *stream) {

	int rc = close(stream->fd);

	free(stream);

	return (rc == 0) ? 0 : EOF;
}


size_t fread(void *ptr, size_t size, size_t count, FILE *stream) {

	if (size == 0 || count == 0)
		return 0;

	char *cptr = ptr;
	size_t elems = 0;

	for (; elems < count; elems++) {

		int rc = read(stream->fd, cptr, size);
		
		if (rc <= 0)
			break;

		cptr += size;
	}

	return elems;
}



size_t fwrite (const void *ptr, size_t size, size_t count, FILE *stream) {

	if (size == 0 || count == 0)
		return 0;

	char *cptr = ptr;
	size_t elems = 0;

	for (; elems < count; elems++) {

		int rc = write(stream->fd, cptr, size);
		
		if (rc != size)
			break;

		cptr += size;
	}

	return elems;
}