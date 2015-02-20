#include <vfs.h>

static vfs_node _root;

vfs_node *vfs_open(vfs_node *n, char *path) {
	return n->open != 0 ? n->open(path) : 0;
}