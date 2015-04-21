#pragma once

typedef void fs_node;

typedef fs_node *(*open_t)(char *path);
typedef int (*write_t)(fs_node *n, const void *buff, size_t off, int count);

typedef struct {
	open_t 	open;
	write_t write;
} fs_calls;

typedef struct {
	fs_node 	*fn;
	fs_calls 	*calls;
} vnode;

typedef struct {
	vnode 	*n;
	size_t 	off;
} file_handle;

fs_node *vfs_open(char *path);