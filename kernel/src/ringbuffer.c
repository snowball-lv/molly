#include <ringbuffer.h>
#include <kalloc.h>
#include <klib.h>


ringbuffer_t *ringbuffer_create(size_t size) {

	ringbuffer_t *rb = kmalloc(sizeof(ringbuffer_t));

	rb->buffer 		= kmalloc(size);
	rb->read_pos 	= 0;
	rb->write_pos 	= 0;
	rb->size 		= size;

	return rb;
}


static void inc_read(ringbuffer_t *rb) {
	rb->read_pos = (rb->read_pos + 1) % rb->size;
}


size_t ringbuffer_read(ringbuffer_t *rb, void *buff, size_t count) {

	size_t read = 0;

	while (read != count) {

		if (ringbuffer_unread(rb) > 0) {
			((char *)buff)[read] = rb->buffer[rb->read_pos];
			inc_read(rb);
			read++;
		}
	}

	return read;
}


size_t ringbuffer_space(ringbuffer_t *rb) {

	if (rb->write_pos < rb->read_pos) {

		return rb->read_pos - rb->write_pos - 1;

	} else if (rb->read_pos < rb->write_pos) {

		return (rb->size - rb->write_pos) + rb->read_pos - 1;

	}

	return rb->size - 1;
}


size_t ringbuffer_unread(ringbuffer_t *rb) {
	return rb->size - ringbuffer_space(rb) - 1;
}


static void inc_write(ringbuffer_t *rb) {
	rb->write_pos = (rb->write_pos + 1) % rb->size;
}


size_t ringbuffer_write(ringbuffer_t *rb, void *buff, size_t count) {

	size_t written = 0;

	while (written != count) {

		if (ringbuffer_space(rb) > 0) {
			rb->buffer[rb->write_pos] = ((char *)buff)[written];
			inc_write(rb);
			written++;
		}
	}

	return written;
}