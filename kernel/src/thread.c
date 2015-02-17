#include <thread.h>

#include <kallocator.h>
#include <paging.h>
#include <scheduler.h>
#include <klib.h>
#include <console.h>

#define STACK_SIZE	(PAGE_SIZE)

void create_thread(int (*entry)()) {
	thread_t *t = alloc_thread(entry);
	schedule_thread(t);
}

thread_t *alloc_thread(int (*entry)()) {

	int *stack = kmalloc(STACK_SIZE);
	int *top = stack + STACK_SIZE / sizeof(int);
	
	top--;
	
	*top = (int)thread_entry;
	
	for (int i = 0; i < 8; i++) {
		top--;
		*top = 0;
	}
	
	thread_t *t = kmalloc(sizeof(thread_t));
	
	t->stack 	= stack;
	t->sp 		= top;
	t->entry 	= entry;
	t->state	= THREAD_STATE_NONE;
	t->next 	= 0;
	
	return t;
}



















