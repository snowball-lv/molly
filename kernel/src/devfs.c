#include <devfs.h>
#include <console.h>
#include <kalloc.h>

static int console_write(	fs_node *fn,
							void *buff,
							size_t off,
							int count)
{
	kprintfln("cons. write: %s", buff);
	return strlen(buff);
}

static idrvr console_drvr = {
	.open = 0,
	.write = console_write
};

static vnode *open(char *path) {
	kprintfln("devfs open: %s", path);
	
	if (strcmp("console", path) == 0) {
		vnode *vn = kmalloc(sizeof(vnode));
		
		vn->type 	= T_DEV;
		vn->drvr	= &console_drvr;
		vn->fn		= 0;
		
		return vn;
	}
	
	return 0;
}

idrvr devfs_drvr = {
	.open = open
};

