#pragma once

#include <paging.h>

void switch_context(void **old_sp, void *new_sp);

void user_mode();

typedef struct {
	int state;
	int *ustack;
	int *usp;
	int *kstack;
	int *ktop;
	int *ksp;
	int (*entry)();
} thread_t;

void set_up_thread(thread_t *t, int (*entry)());

#define MAX_THREADS		(16)
#define S_FREE			(1)
#define S_USED			(2)

typedef struct {
	int 		state;
	pd_t		*pd;
	void 		*brk;
	thread_t 	threads[MAX_THREADS];
	int 		ct_num;
} proc_t;

proc_t *cproc();























