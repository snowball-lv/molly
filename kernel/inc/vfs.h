#pragma once

#include <string.h>

typedef struct vnode vnode;
typedef void fs_node;

typedef struct {
	vnode *(*open)(char *path);
	int (*write)(fs_node *fn, void *buff, size_t off, int count);
} idrvr;

#define T_FILE	1
#define T_DIR	2
#define T_DEV	3

struct vnode {
	idrvr		*drvr;
	int			type;
	fs_node 	*fn;
};

typedef struct {
	int 	state;
	vnode 	*vn;
	size_t 	off;
} file_handle;

vnode *vfs_open(char *path);

void init_vfs();









