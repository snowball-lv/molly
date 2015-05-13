#pragma once

#include <string.h>

typedef struct vnode vnode;
typedef void fs_node;

typedef int (*open_t)(vnode *vn, char *path);

#define T_FILE	1
#define T_DIR	2
#define T_DEV	3

struct vnode {

	int		type;
	fs_node	*fn;

	open_t	open;

};

typedef struct {

	int 	state;
	vnode 	*vn;
	size_t 	off;

} file_handle;

vnode *vfs_open(char *path);

void init_vfs();
void vfs_mount(char *path, vnode *vn);

















