#include <molly.h>

int main(int argc, char **argv);

//try to get rid of this shit
int atexit(void (*func)(void)) { return 0; }

void _molly_entry(int argc, char **argv) {
	main(argc, argv);
	exit();
}

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

int exec(char *path, char **args) {
	return syscall_do(SYS_EXEC, path, args);
}

int open(char *path) {
	return syscall_do(SYS_OPEN, path);
}

void exit() {
	syscall_do(SYS_EXIT);
}

int close(int fd) {
	return syscall_do(SYS_CLOSE, fd);
}

int write(int fd, const void *buff, int count) {
	return syscall_do(SYS_WRITE, fd, buff, count);
}

int	read(int fd, const void *buff, int count) {
	return syscall_do(SYS_READ, fd, buff, count);
}














