#pragma once

typedef struct {
	int state;
} spinlock_t;

void spinlock_init		(spinlock_t *s);
void spinlock_lock		(spinlock_t *s);
void spinlock_unlock	(spinlock_t *s);
