#pragma once

typedef struct {
	int state;
} mutex_t;

void mutex_init(mutex_t *m);

void mutex_lock(mutex_t *m);
void mutex_release(mutex_t *m);