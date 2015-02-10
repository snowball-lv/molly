#pragma once

#include <stdint.h>

typedef uintmax_t tid_t;

struct thread_t {
	tid_t 	id;
	void 	*sp;
	int		(*entry)();
};