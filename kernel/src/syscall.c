#include <syscall.h>
#include <molly.h>
#include <console.h>
#include <string.h>

static void syscall_handler(trapframe_t *tf) {
	kprintfln("syscall %d", tf->eax);
	switch (tf->eax) {
		case SYS_EXEC: sys_exec(tf); break;
		default:
		kprintfln("unknown syscall");
		break;
	}
}

#define I_SYSCALL	(0x80)

void init_syscall() {
	isr_set_handler(I_SYSCALL, syscall_handler);
}

static void *getbp(trapframe_t *tf) {
	uint32_t *bp = (uint32_t *)tf->ebp;
	uint32_t *prev = (uint32_t *)*bp;
	return prev;
}

static void *getarg(trapframe_t *tf, int n) {
	uint32_t *bp = getbp(tf);
	return (void *)*(bp + 2 + n);
}

void sys_exec(trapframe_t *tf) {
	kprintfln("exec");
	char *file 	= getarg(tf, 0);
	char **argv = getarg(tf, 1);
}
























