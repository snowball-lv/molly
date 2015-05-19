#include <devfs.h>
#include <console.h>
#include <kalloc.h>
#include <klib.h>
#include <vfs.h>
#include <debug.h>

#define MAX_DEVICES 	16
#define S_FREE			1
#define S_USED			2

typedef struct {

	int		state;
	char 	*name;
	vnode	*vn;

} dev_t;

static dev_t devices[MAX_DEVICES] = {};

static dev_t *get_free_slot() {

	for (int i = 0; i < MAX_DEVICES; i++) {

		dev_t *dev = &devices[i];

		if(dev->state == S_FREE)
			return dev;
	}
	
	return 0;
}

int dev_add(char *name, vnode *vn) {

	dev_t *dev = get_free_slot();
	
	if (dev == 0) {
		logfln("out of device slots!");
		return -1;
	}
		
	dev->state 	= S_USED;
	dev->name 	= kstrdup(name);
	dev->vn 	= vn;

	return 1;
}

static dev_t *find_dev(char *path) {

	for (int i = 0; i < MAX_DEVICES; i++) {

		dev_t *dev = &devices[i];

		if(dev->state == S_FREE)
			continue;

		if (strcmp(dev->name, path) == 0)
			return dev;
	}

	return 0;
}

static vnode *devfs_get_vnode(char *path) {
	logfln("devfs get vnode: [%s]", path);

	dev_t *dev = find_dev(path);

	if (dev == 0) {
		logfln("device not found: [%s]", path);
		return 0;
	}

	return dev->vn;
}

static vnode devfs = {

	.get_vnode = devfs_get_vnode

};

void init_devfs() {

	for (int i = 0; i < MAX_DEVICES; i++)
		devices[i].state = S_FREE;

	if (vfs_mount("#", &devfs) == -1)
		panic("*** couldn't mount devfs");
}














