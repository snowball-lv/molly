#include <molly.h>

int syscall_do(int f, ...);

void log(const char *msg) {
	syscall_do(SYS_LOG, msg);
}

void *sbrk(int size) {
	return (void *)syscall_do(SYS_SBRK, size);
}

int mkt(int (*entry)()) {
	return syscall_do(SYS_MKT, entry);
}

void yield() {
	syscall_do(SYS_YIELD);
}

int fork() {
	return syscall_do(SYS_FORK);
}

void yieldp() {
	syscall_do(SYS_YIELDP);
}

void stall(unsigned ms) {
	syscall_do(SYS_STALL, ms);
}















