#include <molly.h>

int syscall_do(int f, ...);

int open(const char *name) {
	return syscall_do(SYS_OPEN, name);
}

int read(int fd, void *buf, int count) {
	return syscall_do(SYS_READ, fd, buf, count);
}

int write(int fd, void *buf, int count) {
	return syscall_do(SYS_WRITE, fd, buf, count);
}

int close(int fd) {
	return syscall_do(SYS_CLOSE, fd);
}

void log(const char *msg) {
	syscall_do(SYS_LOG, msg);
}

void *sbrk(int size) {
	return (void *)syscall_do(SYS_SBRK, size);
}
















