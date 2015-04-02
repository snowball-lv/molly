#pragma once

#include <string.h>

#define SYS_OPEN		(1)
#define SYS_READ		(2)
#define SYS_WRITE		(3)
#define SYS_CLOSE		(4)

#define SYS_LOG			(6)

#define SYS_SBRK		(7)

int open(const char *name);
int read(int fd, void *buf, int count);
int write(int fd, void *buf, int count);
int close(int fd);

void log(const char *msg);

void *sbrk(int size);


















