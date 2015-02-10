#pragma once

#include <stdint.h>
#include <vmm.h>

typedef uintmax_t pid_t;

typedef struct proc_t proc_t;

struct proc_t {
	pid_t	id;
	pd_t	*pd;
	void	*heap_base;
	void	*heap_top;
	void	*esp;
	int		(*entry)();
	proc_t 	*next;
};

proc_t *get_kproc();
proc_t *get_current_proc();

void init_kproc();

void reschedule();
void create_process();