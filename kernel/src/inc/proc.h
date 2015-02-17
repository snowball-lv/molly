#pragma once

#include <paging.h>
#include <thread.h>

#define PROC_STATE_NONE	 	(-1)
#define PROC_STATE_RUNNING 	(1)
#define PROC_STATE_DISPOSE	(2)

typedef struct proc_t proc_t;

struct proc_t {

	pd_t 		*pd;
	
	thread_t	*current_thread;
	thread_t	*thread_queue;
	
	int 		state;
	
	proc_t		*next;
};

void init_null_proc();

proc_t *get_null_proc();

void create_process(int (*entry)());















