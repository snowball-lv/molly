#pragma once

#include <string.h>

#define TYPE_DIR	(1)

typedef struct vnode vnode;

typedef vnode *(*open_t)(const char *path);

struct vnode {
	int 	type;
	open_t	open;
};

void init_vfs();

void mount_root(vnode *n);














