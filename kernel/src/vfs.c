#include <vfs.h>
#include <types.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <kalloc.h>
#include <klib.h>
#include <devfs.h>

vnode *vfs_open(char *path) { 
	kprintfln("vfs open: %s", path);
	
	//pass to devfs
	if (path[0] == '#') {
		char *rem = &path[1];
		return devfs_drvr.open(rem);
	}
	
	return 0;
}

void init_vfs() {
	
}





















