#pragma once

typedef struct vfs_node vfs_node;

typedef vfs_node *(* vfs_open_t)(char *path);

struct vfs_node {
	vfs_open_t open;
};