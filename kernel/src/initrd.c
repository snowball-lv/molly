#include <initrd.h>
#include <console.h>
#include <vfs.h>
#include <devfs.h>
#include <klib.h>
#include <types.h>

static vnode initrd;

void init_initrd() {
	kprintfln("init initial ramdisk");

	if (vfs_mount("/", &initrd) == -1)
		panic("unable to mount initrd");
}

extern none_t _RAMFS_START;

static vnode *initrd_get_vnode(char *path) {



	return 0;
}

static vnode initrd = {

	.get_vnode = initrd_get_vnode

};
