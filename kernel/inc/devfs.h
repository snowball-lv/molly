#pragma once

#include <vfs.h>

void init_devfs();
void dev_add(char *name, vnode *vn);