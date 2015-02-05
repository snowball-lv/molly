#pragma once

#include <stdint.h>
#include <vmm.h>

typedef uintmax_t pid_t;

typedef struct {
	pid_t	id;
	pd_t	*pd;
	void	*heap_base;
	void	*heap_top;
} proc_t;

proc_t *get_kproc();
proc_t *get_current_proc();

void init_kproc();