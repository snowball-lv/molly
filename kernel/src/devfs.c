#include <devfs.h>
#include <console.h>
#include <kalloc.h>
#include <klib.h>

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

void dev_add(char *name, vnode *vn) {

	dev_t *dev = get_free_slot();
	
	if (dev == 0)
		panic("out of device slots!");
		
	dev->state 	= S_USED;
	dev->name 	= kstrdup(name);
	dev->vn 	= vn;
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

static int devfs_open(vnode *vn, char *path) {
	kprintfln("devfs open: [%s]", path);

	dev_t *dev = find_dev(path);

	if (dev == 0)
		panic("device not found: [%s]", path);

	char *rem = path_rem(path, dev->name);

	return dev->vn->open(vn, rem);
}

static vnode devfs = {

	.open = devfs_open

};

void init_devfs() {

	for (int i = 0; i < MAX_DEVICES; i++)
		devices[i].state = S_FREE;

	vfs_mount("#", &devfs);
}














