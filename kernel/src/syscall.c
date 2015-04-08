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
#include <idt.h>
#include <gdt.h>
#include <param.h>

static void sys_log		(trapframe_t *tf);
static void sys_sbrk	(trapframe_t *tf);
static void sys_mkt		(trapframe_t *tf);
static void sys_yield	(trapframe_t *tf);
static void sys_fork	(trapframe_t *tf);
static void sys_yieldp	(trapframe_t *tf);

static void syscall_handler(trapframe_t *tf) {

	switch (tf->eax) {
	
		case SYS_LOG: 		sys_log(tf); 	break;
		case SYS_SBRK: 		sys_sbrk(tf); 	break;
		case SYS_MKT: 		sys_mkt(tf); 	break;
		case SYS_YIELD: 	sys_yield(tf); 	break;
		case SYS_FORK: 		sys_fork(tf); 	break;
		case SYS_YIELDP: 	sys_yieldp(tf); break;
		
		default:
		kprintfln("unknown syscall");
		break;
	}
}

#define I_SYSCALL	(0x80)

void init_syscall() {
	set_isr(I_SYSCALL, syscall_handler);
}

static uint32_t *getbp(trapframe_t *tf) {
	uint32_t *bp = (uint32_t *)tf->ebp;
	uint32_t *prev = (uint32_t *)*(bp + 0);
	return prev;
}

#define ARG(tf, num, type)	((type)*(getbp(tf) + 2 + (num)))
#define RET(tf, value)		(tf->eax = (value))

static void sys_log(trapframe_t *tf) {
	kprintfln("log: %s", ARG(tf, 0, const char *));
}

static void sys_sbrk(trapframe_t *tf) {
	int size = ARG(tf, 0, int);
	
	proc_t *p = cproc();
	char *cbrk = p->brk;
	char *nbrk = cbrk + size;
	
	if (size > 0) {
		
		RET(tf, (uintptr_t)cbrk);
	
		for (;cbrk < nbrk; cbrk += PAGE_SIZE) {
			void *phys = pmm_alloc_block();
			map_page(cbrk, phys, PTE_P | PTE_RW | PTE_U);
		}
		
		p->brk = cbrk;
	
	} else if (size < 0) {
	
		kprintfln("negative sbrk not implemented!");
	
	} else {
		RET(tf, (uintptr_t)cbrk);
	}
}

typedef int (*tmain_t)();

static void sys_mkt(trapframe_t *tf) {
	kprintfln("mkt");
	
	proc_t *p = cproc();
	
	thread_t *t = 0;
	for (int i = 0; i < MAX_THREADS; i++) {
		if (p->threads[i].state == S_FREE) {
			t = &p->threads[i];
			kprintfln("new thread id: %d", i);
			RET(tf, i);
			break;
		}
	}
	
	if (t == 0) {
		RET(tf, -1);
		return;
	}
	
	tmain_t tmain = ARG(tf, 0, tmain_t);
	set_up_thread(t, tmain);
}

static void sys_yield(trapframe_t *tf) {
	kprintfln("yield");
	
	proc_t *p 		= cproc();
	thread_t *ct 	= &p->threads[p->ct_num];
	thread_t *nt 	= 0;
	
	kprintfln("from: %d", p->ct_num);
	
	for (int i = 1; i < MAX_THREADS; i++) {
		int mi = (p->ct_num + i) % MAX_THREADS;
		if (p->threads[mi].state == S_USED) {
			nt = &p->threads[mi];
			p->ct_num = mi;
			break;
		}
	}
	
	if (nt == 0)
		return;
		
	kprintfln("to: %d", p->ct_num);
	
	set_tss(nt->ktop);
	switch_context((void **)&ct->ksp, nt->ksp);
}

int proc_clone(trapframe_t *tf);

static void sys_fork(trapframe_t *tf) {
	kprintfln("fork");

	int num = proc_clone(tf);
	
	kprintfln("new proc: %d", num);
	
	RET(tf, num);
}

void *vtp(void *virt);
proc_t *get_proc(size_t num);
void load_PDBR(pd_t *pd);

static void sys_yieldp(trapframe_t *tf) {
	kprintfln("yield proc");
	
	proc_t *cp 		= cproc();
	proc_t *np 		= get_proc(1);
	thread_t *ct 	= &cp->threads[cp->ct_num];
	thread_t *nt 	= &np->threads[np->ct_num];
	
	load_PDBR(vtp(np->pd));
	
	set_tss(nt->ktop);
	switch_context((void **)&ct->ksp, nt->ksp);
}



















