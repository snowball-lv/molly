#include <syscall.h>
#include <molly.h>
#include <console.h>
#include <string.h>
#include <proc.h>
#include <string.h>
#include <kalloc.h>
#include <molly.h>
#include <paging.h>
#include <pmm.h>

void sys_open(trapframe_t *tf);
void sys_read(trapframe_t *tf);
void sys_write(trapframe_t *tf);
void sys_close(trapframe_t *tf);

void sys_log(trapframe_t *tf);

void sys_sbrk(trapframe_t *tf);

static void syscall_handler(trapframe_t *tf) {

	switch (tf->eax) {
	
		case SYS_OPEN: 		sys_open(tf); 	break;
		case SYS_READ: 		sys_read(tf); 	break;
		case SYS_WRITE: 	sys_write(tf); 	break;
		case SYS_CLOSE: 	sys_close(tf); 	break;
		
		case SYS_LOG: 		sys_log(tf); 	break;
		
		case SYS_SBRK: 		sys_sbrk(tf); 	break;
		
		default:
		kprintfln("unknown syscall");
		break;
	}
}

#define I_SYSCALL	(0x80)

void init_syscall() {
	isr_set_handler(I_SYSCALL, syscall_handler);
}

static uint32_t *getbp(trapframe_t *tf) {
	uint32_t *bp = (uint32_t *)tf->ebp;
	uint32_t *prev = (uint32_t *)*(bp + 0);
	return prev;
}

#define ARG(tf, num, type)	((type)*(getbp(tf) + 2 + (num)))
#define RET(tf, value)		(tf->eax = (value))

void sys_open(trapframe_t *tf) {

	const char *path = ARG(tf, 0, const char *);
	kprintfln("open: %s", path);
}

void sys_read(trapframe_t *tf) {
	int count = ARG(tf, 2, int);
	kprintfln("read: %d", count);
}

void sys_write(trapframe_t *tf) {
	int count = ARG(tf, 2, int);
	kprintfln("write: %d", count);
}

void sys_close(trapframe_t *tf) {

	int fdi = ARG(tf, 0, int);
	kprintfln("close: %d", fdi);
}

void sys_log(trapframe_t *tf) {
	kprintfln("log: %s", ARG(tf, 0, const char *));
}

void sys_sbrk(trapframe_t *tf) {
	int size = ARG(tf, 0, int);
	
	xproc_t *p = cproc();
	char *cbrk = p->brk;
	char *nbrk = cbrk + size;
	
	if (size > 0) {
		
		RET(tf, (uintptr_t)cbrk);
	
		for (;cbrk < nbrk; cbrk += PAGE_SIZE) {
			void *phys = pmm_alloc_page();
			map_page(cbrk, phys, PTE_P | PTE_RW | PTE_U);
		}
		
		p->brk = cbrk;
	
	} else if (size < 0) {
	
		kprintfln("negative sbrk not implemented!");
	
	} else {
		RET(tf, (uintptr_t)cbrk);
	}
}



















