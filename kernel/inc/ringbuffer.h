#pragma once

#include <string.h>

typedef struct {

	char 	*buffer;
	size_t 	read_pos;
	size_t 	write_pos;
	size_t 	size;

} ringbuffer_t;

ringbuffer_t *ringbuffer_create(size_t size);

size_t ringbuffer_read(ringbuffer_t *rb, void *buff, size_t count);
size_t ringbuffer_space(ringbuffer_t *rb);
size_t ringbuffer_unread(ringbuffer_t *rb);
size_t ringbuffer_write(ringbuffer_t *rb, void *buff, size_t count);