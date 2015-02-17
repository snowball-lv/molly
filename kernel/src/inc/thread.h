#pragma once

#define THREAD_STATE_NONE	 	(-1)
#define THREAD_STATE_RUNNING 	(1)
#define THREAD_STATE_DISPOSE	(2)

typedef struct thread_t thread_t;

struct thread_t {
	void 		*stack;
	void 		*sp;
	int 		(*entry)();
	int 		state;
	thread_t 	*next;
};

thread_t *alloc_thread(int (*entry)());

void create_thread(int (*entry)());