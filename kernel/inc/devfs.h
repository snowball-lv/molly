#pragma once

#include <vfs.h>

void init_devfs();
int dev_add(char *name, vnode *vn);