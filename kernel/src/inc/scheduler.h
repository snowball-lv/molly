#pragma once

#include <proc.h>

void init_scheduler();

void reschedule();

//scheduler_asm.asm
void switch_context(void **old_sp, void *new_sp);

void schedule_thread(thread_t *t);

void thread_entry();

void schedule_process(proc_t *p);