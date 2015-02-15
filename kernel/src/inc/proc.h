#pragma once

#include <paging.h>

typedef struct proc_t proc_t;

struct proc_t {
	pd_t *pd;
};

void init_null_proc();

proc_t *get_null_proc();