#include <pipe.h>
#include <string.h>
#include <kalloc.h>
#include <sync.h>
#include <klib.h>

#define PIPE_SIZE	16

typedef struct {

	char 		buffer[PIPE_SIZE];
	int 		write_pos;
	int 		read_pos;
	spinlock_t 	lock;

	int 		ref_cnt;

} pipe_node;

static int pipe_open(vnode *vn) {
	kprintfln("pipe open");

	pipe_node *pn = (pipe_node *)vn->fn;
	spinlock_lock(&pn->lock);
	pn->ref_cnt++;
	spinlock_unlock(&pn->lock);
	
	return 1;
}

static int pipe_close(fs_node *fn) {
	kprintfln("pipe close");

	pipe_node *pn = (pipe_node *)fn;
	spinlock_lock(&pn->lock);
	pn->ref_cnt--;
	spinlock_unlock(&pn->lock);

	return 0;
}

//leave one byte between read and write positions
static int pipe_space(pipe_node *pn) {

	if (pn->write_pos < pn->read_pos) {

		return pn->read_pos - pn->write_pos - 1;

	} else if (pn->read_pos < pn->write_pos) {

		return (PIPE_SIZE - pn->write_pos) + pn->read_pos - 1;

	}

	//equal
	return PIPE_SIZE - 1;
}

static void pipe_inc_write(pipe_node *pn) {
	pn->write_pos = (pn->write_pos + 1) % PIPE_SIZE;
}

static int pipe_write(fs_node *fn, void *buff, size_t off, int count) {
	kprintfln("pipe write");
	pipe_node *pn = (pipe_node *)fn;

	if (count < 0)
		panic("write size can't be: %d", count);

	int written = 0;

	while (written != count) {
		spinlock_lock(&pn->lock);

		if (pipe_space(pn) == 0)
			panic("pipe space is 0");

		while (pipe_space(pn) > 0 && written < count) {
			pn->buffer[pn->write_pos] = ((char *)buff)[written];
			pipe_inc_write(pn);
			written++;
		}

		spinlock_unlock(&pn->lock);
	}

	return written;
}

static int pipe_unread(pipe_node *pn) {
	return PIPE_SIZE - pipe_space(pn) - 1;
}

static void pipe_inc_read(pipe_node *pn) {
	pn->read_pos = (pn->read_pos + 1) % PIPE_SIZE;
}

static int pipe_read(fs_node *fn, void *buff, size_t off, int count) {
	kprintfln("pipe read");
	pipe_node *pn = (pipe_node *)fn;

	if (count < 0)
		panic("read size can't be: %d", count);

	int read = 0;

	while (read != count) {
		spinlock_lock(&pn->lock);

		//kprintfln("r: %d, w: %d", pn->read_pos, pn->write_pos);
		//kprintfln("pipe unread: %d", pipe_unread(pn));
		//panic("***");

		while (pipe_unread(pn) > 0 && read < count) {
			((char *)buff)[read] = pn->buffer[pn->read_pos];
			pipe_inc_read(pn);
			read++;
		}

		spinlock_unlock(&pn->lock);
	}

	return read;
}

vnode *make_pipe() {
	kprintfln("make pipe");

	pipe_node *pn = kmalloc(sizeof(pipe_node));

	pn->ref_cnt 	= 1;
	pn->write_pos 	= 0;
	pn->read_pos	= 0;

	spinlock_init(&pn->lock);

	vnode *vn = kmalloc(sizeof(vnode));

	vn->fn = pn;

	vn->open 	= pipe_open;
	vn->close 	= pipe_close;
	vn->write 	= pipe_write;
	vn->read 	= pipe_read;

	return vn;
}