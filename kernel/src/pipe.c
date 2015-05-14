#include <pipe.h>
#include <string.h>
#include <kalloc.h>
#include <sync.h>
#include <klib.h>

#define PIPE_SIZE	4096

typedef struct {

	char 		buffer[PIPE_SIZE];
	int 		pos;
	spinlock_t 	lock;

} pipe_node;

static int pipe_open(vnode *vn, char *path) {
	return 0;
}

static int pipe_close(fs_node *fn) {
	pipe_node *pn = (pipe_node *)fn;
	kfree(pn);
	return 0;
}

static int pipe_write(fs_node *fn, void *buff, size_t off, int count) {
	pipe_node *pn = (pipe_node *)fn;
	spinlock_lock(&pn->lock);

	if (count > PIPE_SIZE)
		panic("write size too big for pipe. %d / %d", count, PIPE_SIZE);

	//wait for pipe to make enough room
	if (pn->pos + count > PIPE_SIZE) {

	}


	spinlock_unlock(&pn->lock);
}

vnode *make_pipe() {

	pipe_node *pn = kmalloc(sizeof(pipe_node));
	pn->pos = 0;
	spinlock_init(&pn->lock);

	vnode *vn = kmalloc(sizeof(vnode));

	vn->fn = pn;

	vn->open 	= pipe_open;
	vn->close 	= pipe_close;
	vn->write 	= pipe_write;

	return vn;
}