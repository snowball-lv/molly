#include <molly.h>

int syscall_do(int f, ...);

int exec(const char *file, const char **argv) {
	return syscall_do(SYS_EXEC, file, argv);
}