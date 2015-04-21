#include <vfs.h>
#include <types.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <kalloc.h>

fs_node *console_open(char *path) {
	kprintfln("opening console");
	return 0;
}

fs_node *devfs_open(char *path) {
	kprintfln("devfs open: %s", path);
	if (!strcmp("console", path)) {
		return console_open(0);
	}
	return 0;
}

static fs_calls devfs_calls = {
	.open = devfs_open
};

fs_node *vfs_open(char *path) { 
	kprintfln("vfs open: %s", path);
	
	if (path == 0)
		return 0;
		
	if (path[0] == '#') {
		//device
		return devfs_calls.open(&path[1]);
	}
	
	return 0;
}




















