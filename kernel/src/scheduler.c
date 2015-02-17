#include <scheduler.h>

#include <console.h>
#include <proc.h>
#include <klib.h>
#include <paging.h>
#include <sync.h>
#include <thread.h>

static proc_t 	*_current_proc;
static proc_t	*_proc_queue;
static mutex_t 	_schedule_m;

static thread_t *_dispose_queue;

void init_scheduler() {
	mutex_init(&_schedule_m);
	
	_current_proc 	= get_null_proc();
	_proc_queue 	= 0;
	_dispose_queue	= 0;
}

static void enqueue_dispose(thread_t *t) {

	t->next = 0;

	if (_dispose_queue == 0) {
		_dispose_queue = t;
		return;
	}
	
	thread_t *tmp = _dispose_queue;
	while (tmp->next != 0)
		tmp = tmp->next;
		
	tmp->next = t;
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
	
	if (t->state == THREAD_STATE_DISPOSE) {
		enqueue_dispose(t);
		return ;
	}

	if (p->thread_queue == 0) {
		p->thread_queue = t;
		return;
	}
	
	thread_t *tmp = p->thread_queue;
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

	if (_proc_queue == 0) {
		_proc_queue = p;
		return;
	}
	
	proc_t *tmp = _proc_queue;
	while (tmp->next != 0)
		tmp = tmp->next;
		
	tmp->next = p;
}

static void switch_thread(thread_t *from, thread_t *to) {
	
	_current_proc->current_thread = to;
	
	switch_context(&from->sp, to->sp);
}

static void switch_process(proc_t *from, proc_t *to) {
	
	void *pd_phys = virt_to_phys(to->pd);
	
	load_PDBR(pd_phys);
	
	thread_t *from_thread = from->current_thread;
	enqueue_thread(from, from_thread);
	from->current_thread = 0;
	
	thread_t *to_thread = dequeue_thread(to);
	
	_current_proc = to;
	
	switch_thread(from_thread, to_thread);
}

//delete disposable objects
static void sanitize() {

}

void reschedule() {
	disable_ints();
	
	sanitize();
	
	enqueue_proc(_current_proc);
	proc_t *p = dequeue_proc();
	
	if (p != _current_proc) {
	
		switch_process(_current_proc, p);
		
	} else {
	
		thread_t *c = p->current_thread;
		enqueue_thread(p, c);
		thread_t *t = dequeue_thread(p);
		
		if (c != t)
			switch_thread(c, t);
			
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
	
	t->state = THREAD_STATE_DISPOSE;
	
	reschedule();
}

void schedule_process(proc_t *p) {
	mutex_lock(&_schedule_m);
	
	disable_ints();
	enqueue_proc(p);
	enable_ints();
	
	mutex_release(&_schedule_m);
}






























