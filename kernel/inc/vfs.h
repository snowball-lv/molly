#pragma once

#include <string.h>

typedef struct vnode vnode;
typedef void fs_node;

#define T_FILE	1
#define T_DIR	2
#define T_DEV	3

struct vnode {

	int			type;
	fs_node		*fn;

	int (*open)(vnode *vn, char *path);
	int (*close)(fs_node *fn);
	int (*write)(fs_node *fn, void *buff, size_t off, int count);

};

typedef struct {

	int 	state;
	vnode 	*vn;
	size_t 	off;

} file_handle;

vnode *vfs_open(char *path);
void vfs_close(vnode *vn);
int vfs_write(vnode *vn, void *buff, size_t off, int count);

void init_vfs();
void vfs_mount(char *path, vnode *vn);

















