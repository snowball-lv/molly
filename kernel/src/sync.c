#include <sync.h>

int mutex_test(int *state);

void mutex_lock(mutex_t *m) {
	while (!mutex_test(&m->state));
}

void mutex_release(mutex_t *m) {
	//replace with xchg to avoid ordering bugs
	m->state = 1;
}

void mutex_init(mutex_t *m) {
	m->state = 1;
}