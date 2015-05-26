#pragma once

#include <paging.h>
#include <vfs.h>

void switch_context(void **old_sp, void *new_sp);

void user_mode();
void run_init();

typedef void (*entry_f)(int argc, char **argv);

typedef struct {
	
	int 	state;
	int 	*ustack;
	int 	*usp;
	int 	*kstack;
	int 	*ktop;
	int 	*ksp;
	entry_f entry;

} thread_t;

void set_up_thread(thread_t *t, entry_f entry, void *stack);

#define MAX_THREADS		16
#define S_FREE			1
#define S_USED			2
#define MAX_FILES		16

typedef struct {

	int 		state;
	pd_t		*pd;
	void 		*brk;
	thread_t 	threads[MAX_THREADS];
	int 		ct_num;
	char 		*cwd;
	file_handle	files[MAX_FILES];

} proc_t;

proc_t *cproc();
proc_t *get_proc(size_t num);

int create_proc(char *wd, char *img);

void reschedule();



















