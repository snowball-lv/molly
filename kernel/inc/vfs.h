#pragma once

#include <string.h>
#include <scancodes.h>
#include <molly.h>

typedef struct vnode vnode;
typedef void fs_node;

#define T_FILE	1
#define T_DIR	2
#define T_DEV	3
#define T_PIPE	4

struct vnode {

	int			type;
	fs_node		*fn;

	vnode *(*get_vnode)(char *path);

	int (*open)(vnode *vn);
	int (*close)(fs_node *fn);
	int (*read)(fs_node *fn, void *buff, size_t off, int count);
	int (*write)(fs_node *fn, void *buff, size_t off, int count);

	int (*read_event)(fs_node *fn, key_event *e);

	int (*read_dir)(fs_node *fn, size_t off, dirent_t *e);

	int (*seek)(fs_node *fn, size_t cur, int off, seek_t whence);
};

typedef struct {

	int 	state;
	vnode 	*vn;
	size_t 	off;

} file_handle;

vnode *vfs_open(char *path);
int vfs_close(vnode *vn);
int vfs_write(vnode *vn, void *buff, size_t off, int count);
int vfs_read(vnode *vn, void *buff, size_t off, int count);

void init_vfs();
int vfs_mount(char *path, vnode *fs);
















