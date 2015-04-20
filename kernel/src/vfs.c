#include <vfs.h>
#include <types.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <kalloc.h>

extern none_t _RAMFS_START;

#define ROUND_UP(v)	((char *)(((uintptr_t)(v) + 7) & ~(7)))

typedef void fs_node;

typedef struct {
	char *start;
} ramfs_node;

size_t ramfs_read(fs_node *n, void *buff, size_t count) {
	kprintfln("ramfs read");
	ramfs_node *f = n;
	
	char *ptr = f->start;
	kprintfln("%s", ptr);
	ptr += strlen(ptr) + 1;
	ptr = ROUND_UP(ptr);
	ptr += 4;
	ptr = ROUND_UP(ptr);
	
	memcpy(buff, ptr, count);
	
	return 0;
}

fs_node *ramfs_open(char *path) {

	char *ptr = (char *)&_RAMFS_START;
	
	size_t count = *(size_t *)ptr;
	ptr += 4;
	
	for (size_t i = 0; i < count; i++) {
		ptr = ROUND_UP(ptr);
		char *name = ptr;
		
		if (!strcmp(path, name)) {
			//file found
			ramfs_node *n = kmalloc(sizeof(ramfs_node));
			n->start = ptr;
			return n;
		}
		
		ptr += strlen(name) + 1;
		ptr = ROUND_UP(ptr);
		size_t size = *(size_t *)ptr;
		ptr += 4;
		ptr = ROUND_UP(ptr);
		ptr += size;
	}
	
	return 0;
}






















