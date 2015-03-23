#pragma once

#include <interrupt.h>

void init_syscall();

void sys_exec(trapframe_t *tf);