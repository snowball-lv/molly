#include <scheduler.h>

#include <console.h>
#include <proc.h>
#include <klib.h>
#include <paging.h>
#include <sync.h>
#include <thread.h>

static proc_t 	*_current_proc;
static mutex_t 	_schedule_m;

static proc_t	*_proc_queue;
static proc_t 	*_proc_dispose_queue;

static thread_t *_thread_dispose_queue;

void init_scheduler() {
	mutex_init(&_schedule_m);
	
	_current_proc 			= get_null_proc();
	_proc_queue 			= 0;
	_proc_dispose_queue		= 0;
	_thread_dispose_queue 	= 0;
}

static thread_t *dequeue_thread(proc_t *p) {

	thread_t *t = p->thread_queue;
	
	if (t == 0)
		return 0;
		
	p->thread_queue = t->next;
	t->next = 0;
	
	return t;
}

static void enqueue_thread(proc_t *p, thread_t *t) {

	t->next = 0;
	
	thread_t **queue = &p->thread_queue;
	
	if (t->state == THREAD_STATE_DISPOSE) {
		//kprintfln("disposing of thread");
		queue = &_thread_dispose_queue;
	}

	if (*queue == 0) {
		*queue = t;
		return;
	}
	
	thread_t *tmp = *queue;
	while (tmp->next != 0)
		tmp = tmp->next;
		
	tmp->next = t;
}

static proc_t *dequeue_proc() {
	proc_t *p = _proc_queue;
	_proc_queue = p->next;
	p->next = 0;
	return p;
}

static void enqueue_proc(proc_t *p) {

	p->next = 0;
	
	proc_t **queue = &_proc_queue;
	
	if (p->state == PROC_STATE_DISPOSE) {
		//kprintfln("disposing of proc");
		queue = &_proc_dispose_queue;
	}

	if (*queue == 0) {
		*queue = p;
		return;
	}
	
	proc_t *tmp = *queue;
	while (tmp->next != 0)
		tmp = tmp->next;
		
	tmp->next = p;
}

static void switch_thread(thread_t *from, thread_t *to) {

	//kprintfln("switch thread");
	
	_current_proc->current_thread = to;
	
	switch_context(&from->sp, to->sp);
}

static void switch_process(proc_t *from, proc_t *to) {
	
	//kprintfln("switch proc");
	
	void *pd_phys = virt_to_phys(to->pd);
	
	load_PDBR(pd_phys);
	
	thread_t *from_thread = from->current_thread;
	from->current_thread = 0;
	
	thread_t *to_thread = dequeue_thread(to);
	
	_current_proc = to;
	
	switch_thread(from_thread, to_thread);
}

static void sanitize() {

	thread_t *t = _thread_dispose_queue;
	while (t != 0) {
		kfree(t->stack);
		kfree(t);
		t = t->next;
	}
	_thread_dispose_queue = 0;
	
	proc_t *p = _proc_dispose_queue;
	while (p != 0) {
		kfree(p->pd);
		kfree(p);
		p = p->next;
	}
	_proc_dispose_queue = 0;
}

void reschedule() {
	disable_ints();
	
	sanitize();
	
	thread_t *ct = _current_proc->current_thread;
	enqueue_thread(_current_proc, ct);
	
	enqueue_proc(_current_proc);
	proc_t *p = dequeue_proc();
	
	if (p != _current_proc) {
	
		switch_process(_current_proc, p);
		
	} else {
	
		thread_t *t = dequeue_thread(p);
		
		if (ct != t)
			switch_thread(ct, t);
	}
	
	enable_ints();
}

void schedule_thread(thread_t *t) {
	mutex_lock(&_schedule_m);
	
	t->state = THREAD_STATE_RUNNING;
	
	disable_ints();
	enqueue_thread(_current_proc, t);
	enable_ints();
	
	mutex_release(&_schedule_m);
}

static size_t queued_threads(proc_t *p) {

	thread_t *t 	= p->thread_queue;
	size_t count 	= 0;
	
	while (t != 0) {
		count++;
		t = t->next;
	}
	
	return count;
}

void thread_entry() {

	enable_ints();
	
	//run thread
	thread_t *t = _current_proc->current_thread;

	t->entry();
	
	disable_ints();
	
	t->state = THREAD_STATE_DISPOSE;
	
	if (queued_threads(_current_proc) == 0)
		_current_proc->state = PROC_STATE_DISPOSE;

	reschedule();
}

void schedule_process(proc_t *p) {
	mutex_lock(&_schedule_m);
	
	disable_ints();
	enqueue_proc(p);
	enable_ints();
	
	mutex_release(&_schedule_m);
}






























