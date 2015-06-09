#include <molly.h>

int main(int argc, char **argv);

//try to get rid of this shit
int atexit(void (*func)(void)) { return 0; }

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

int	exec(char *path, int in, int out, int err) {
	return syscall_do(SYS_EXEC, path, in, out, err);
}

int open(char *path) {
	return syscall_do(SYS_OPEN, path);
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

size_t getcwd(void *buff) {
	return syscall_do(SYS_GET_CWD, buff);
}

int	readdir(int fd, dirent_t *e) {
	return syscall_do(SYS_READ_DIR, fd, e);
}

void exit_t() {
	syscall_do(SYS_EXIT_T);
}

void exit_p() {
	syscall_do(SYS_EXIT_P);
}

int wait() {
	return syscall_do(SYS_WAIT);
}

void waitpid(int pid) {
	syscall_do(SYS_WAIT_PID, pid);
}

