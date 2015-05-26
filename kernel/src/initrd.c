#include <initrd.h>
#include <console.h>
#include <vfs.h>
#include <devfs.h>
#include <klib.h>
#include <types.h>
#include <kalloc.h>
#include <debug.h>
#include <molly.h>

static vnode initrd;

void init_initrd() {
	kprintfln("init initial ramdisk");

	if (vfs_mount("/", &initrd) == -1)
		panic("unable to mount initrd");
}

extern none_t _RAMFS_START;

typedef struct {

	char 	*data;
	size_t 	size;

} initrd_node;

#define ROUND_UP(v)	((char *)(((uintptr_t)(v) + 7) & ~(7)))

static void *find_file(char *path) {
	
	char *ptr = (char *)&_RAMFS_START;
	size_t count = *(size_t *)ptr;
	ptr += 4;
	
	for (size_t i = 0; i < count; i++) {
	
		ptr = ROUND_UP(ptr);
		char *start = ptr;
		char *name = ptr;
		ptr += strlen(name) + 1;
		
		ptr = ROUND_UP(ptr);
		uint32_t init_size = *(size_t *)ptr;
		ptr += 4;
		
		ptr = ROUND_UP(ptr);
		ptr += init_size;
		
		if (strcmp(path, name) == 0)
			return start;
	}

	return 0;
}

static vnode *initrd_get_vnode(char *path) {
	logfln("initrd get_vnode: [%s] : %d", path, strlen(path));

	initrd_node *fn = 0;

	if (strlen(path) == 0) {
		//root dir
		fn = kmalloc(sizeof(initrd_node));
	} else {
		//normal file
		char *ptr = find_file(path);

		if (ptr == 0)
			return 0;

		fn = kmalloc(sizeof(initrd_node));
		
		ptr += strlen(ptr) + 1;
		ptr = ROUND_UP(ptr);

		fn->size = *(size_t *)ptr;
		
		ptr += 4;
		ptr = ROUND_UP(ptr);

		fn->data = ptr;
	}

	if (fn == 0)
		return 0;

	vnode *vn = kmalloc(sizeof(vnode));

	vn->fn = fn;

	vn->open = initrd.open;
	vn->close = initrd.close;
	vn->read = initrd.read;
	vn->write = initrd.write;

	vn->read_dir = initrd.read_dir;

	vn->seek = initrd.seek;

	return vn;
}

static int initrd_open(vnode *vn) {
	return 1;
}

static int initrd_close(fs_node *fn) {
	return 0;
}

static int initrd_read(fs_node *fn, void *buff, size_t off, int count) {
	logfln("initrd read: %d, %d", off, count);

	initrd_node *in = (initrd_node *)fn;

	memcpy(buff, in->data + off, count);

	return count;
}

static int initrd_write(fs_node *fn, void *buff, size_t off, int count) {
	return 0;
}

static int initrd_seek(fs_node *fn, size_t cur, int off, seek_t whence) {

	initrd_node *in = (initrd_node *)fn;

	switch (whence) {
		case SEEK_SET: return off;
		case SEEK_CUR: return cur + off;
		case SEEK_END: return in->size + off;
	}

	return -1;
}

#define ROUND_UP(v)	((char *)(((uintptr_t)(v) + 7) & ~(7)))

static int initrd_read_dir(fs_node *fn, size_t off, dirent_t *e) {
	logfln("initrd read dir: %d", off);

	char *ptr = (char *)&_RAMFS_START;
	size_t count = *(size_t *)ptr;
	ptr += 4;
	
	if (off >= count)
		return 0;

	char *name = 0;

	for (size_t i = 0; i <= off; i++) {
	
		ptr = ROUND_UP(ptr);
		name = ptr;
		ptr += strlen(name) + 1;
		
		ptr = ROUND_UP(ptr);
		uint32_t init_size = *(size_t *)ptr;
		ptr += 4;
		
		ptr = ROUND_UP(ptr);
		ptr += init_size;
	}

	strcpy(e->name, name);

	return 1;
}

static vnode initrd = {

	.get_vnode = initrd_get_vnode,

	.open = initrd_open,
	.close = initrd_close,
	.read = initrd_read,
	.write = initrd_write,

	.read_dir = initrd_read_dir,

	.seek = initrd_seek
};

