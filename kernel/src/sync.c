#include <sync.h>

int spinlock_xchg(int *state, int value);

void spinlock_lock(spinlock_t *s) {
	while (spinlock_xchg(&s->state, 0) != 1);
}

void spinlock_unlock(spinlock_t *s) {
	spinlock_xchg(&s->state, 1);
}

void spinlock_init(spinlock_t *s) {
	s->state = 1;
}