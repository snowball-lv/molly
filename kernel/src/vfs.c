#include <vfs.h>

#include <kallocator.h>
#include <klib.h>
#include <console.h>

vnode *_root = 0;

void mount_root(vnode *n) {
	_root = n;
}

void init_vfs() {

}












