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
#include <pit.h>
#include <debug.h>

static void sys_log		(trapframe_t *tf);
static void sys_sbrk	(trapframe_t *tf);
static void sys_mkt		(trapframe_t *tf);
static void sys_yield	(trapframe_t *tf);
static void sys_fork	(trapframe_t *tf);
static void sys_yieldp	(trapframe_t *tf);
static void sys_stall	(trapframe_t *tf);
static void sys_exec	(trapframe_t *tf);
static void sys_open	(trapframe_t *tf);
static void sys_close	(trapframe_t *tf);
static void sys_write	(trapframe_t *tf);
static void sys_read	(trapframe_t *tf);
static void sys_getcwd	(trapframe_t *tf);
static void sys_readdir	(trapframe_t *tf);
static void sys_exit_t	(trapframe_t *tf);
static void sys_exit_p	(trapframe_t *tf);
static void sys_wait	(trapframe_t *tf);
static void sys_waitpid	(trapframe_t *tf);

static void syscall_handler(trapframe_t *tf) {

	switch (tf->eax) {
	
		case SYS_LOG: 		sys_log(tf); 		break;
		case SYS_SBRK: 		sys_sbrk(tf); 		break;
		case SYS_MKT: 		sys_mkt(tf); 		break;
		case SYS_YIELD: 	sys_yield(tf); 		break;
		case SYS_FORK: 		sys_fork(tf); 		break;
		case SYS_YIELDP: 	sys_yieldp(tf); 	break;
		case SYS_STALL: 	sys_stall(tf); 		break;
		case SYS_EXEC: 		sys_exec(tf); 		break;
		case SYS_OPEN: 		sys_open(tf); 		break;
		case SYS_CLOSE: 	sys_close(tf); 		break;
		case SYS_WRITE: 	sys_write(tf); 		break;
		case SYS_READ: 		sys_read(tf); 		break;
		case SYS_GET_CWD: 	sys_getcwd(tf); 	break;
		case SYS_READ_DIR: 	sys_readdir(tf);	break;
		case SYS_EXIT_T: 	sys_exit_t(tf);		break;
		case SYS_EXIT_P: 	sys_exit_p(tf);		break;
		case SYS_WAIT: 		sys_wait(tf);		break;
		case SYS_WAIT_PID: 	sys_waitpid(tf);	break;
		
		default:
		panic("unknown syscall");
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
#define SYSRET(value) 		{ RET(tf, (value)); return; }

static void sys_log(trapframe_t *tf) {
	char *msg = ARG(tf, 0, char *);
	kprintfln("log: %s", msg);
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
	
	entry_f tmain = ARG(tf, 0, entry_f);
	set_up_thread(t, tmain, sbrk(4096));
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

static void sys_stall(trapframe_t *tf) {
	unsigned ms = ARG(tf, 0, unsigned);
	
	clock_t mspt = 1000 / CLOCKS_PER_SEC;
	
	clock_t min = mspt;
	unsigned adj = ((ms + min - 1) / min) * min;
	//kprintfln("stall %d ms (%d)", ms, adj);
	
	clock_t now = ticks();
	clock_t target = now + adj / mspt;
	//kprintfln("%d -> %d", now, target);
	while (ticks() < target)
		__asm__("hlt");
}

static void sys_exec(trapframe_t *tf) {

	char *path 	= ARG(tf, 0, char *);
	int in 		= ARG(tf, 1, int);
	int out 	= ARG(tf, 2, int);
	int err 	= ARG(tf, 3, int);

	logfln("exec: [%s], %d, %d, %d", path, in, out, err);

	vnode *vn = vfs_open(path);

	if (vn == 0) {
		logfln("couldn't open: [%s]", path);
		SYSRET(-1);
	}

	int size = vn->seek(vn->fn, 0, 0, SEEK_END);

	if (size < 0) {
		logfln("seek fail");
		SYSRET(-1);
	}

	logfln("seek size: %d", size);

	void *img = kmalloc(size);
	int read = vfs_read(vn, img, 0, size);

	logfln("bytes read: %d", read);

	int pid = create_proc("/", img, in, out, err);

	if (pid < 0) {
		logfln("failed to create new proc");
		SYSRET(-1);
	}

	logfln("new pid: %d", pid);

	SYSRET(pid);
}

static int find_free_handle(proc_t *p) {

	for (int i = 0; i < MAX_FILES; i++) {

		file_handle *fh = &p->files[i];

		if (fh->state == S_FREE)
			return i;
	}

	return -1;
}

static void sys_open(trapframe_t *tf) {

	char *path = ARG(tf, 0, char *);
	logfln("sys open: [%s]", path);

	proc_t *p = cproc();
	int fhi = find_free_handle(p);

	if (fhi < 0) {
		logfln("out of free process file handles");
		SYSRET(-1);
	}
	
	vnode *vn = vfs_open(path);
	
	if (vn == 0) {
		logfln("file not found: [%s]", path);
		SYSRET(-1);
	}

	file_handle *fh = &p->files[fhi];
	
	fh->state 	= S_USED;
	fh->vn 		= vn;
	fh->off		= 0;
	
	logfln("opened fd: %d", fhi);
	
	SYSRET(fhi);
}

static void sys_close(trapframe_t *tf) {

	int fd = ARG(tf, 0, int);
	logfln("close fd: %d", fd);
	
	proc_t *p = cproc();
	file_handle *fh = &p->files[fd];

	int rc = vfs_close(fh->vn);

	fh->state = S_FREE;
	
	SYSRET(rc);
}

static void sys_write(trapframe_t *tf) {

	int fd 		= ARG(tf, 0, int);
	void *buff 	= ARG(tf, 1, void *);
	int count 	= ARG(tf, 2, int);
	
	//logfln("write fd: %d", fd);
	
	proc_t *p = cproc();
	file_handle *fh = &p->files[fd];

	vnode *vn = fh->vn;
	
	int r = vfs_write(vn, buff, fh->off, count);
	
	if (r > -1)
		fh->off += r;
	
	RET(tf, r);
}

static void sys_read(trapframe_t *tf) {

	int fd 		= ARG(tf, 0, int);
	void *buff 	= ARG(tf, 1, void *);
	int count 	= ARG(tf, 2, int);
	
	//logfln("read fd: %d", fd);
	
	proc_t *p = cproc();
	file_handle *fh = &p->files[fd];

	vnode *vn = fh->vn;
	
	int r = vfs_read(vn, buff, fh->off, count);
	
	if (r > -1)
		fh->off += r;
	
	RET(tf, r);
}

static void sys_getcwd(trapframe_t *tf) {

	void *buff = ARG(tf, 0, void *);

	proc_t *p = cproc();
	size_t len = strlen(p->cwd);

	if (buff == 0)
		SYSRET(len);

	strcpy(buff, p->cwd);

	SYSRET(len);
}

static void sys_readdir	(trapframe_t *tf) {

	int fd 		= ARG(tf, 0, int);
	dirent_t *e = ARG(tf, 1, dirent_t *);

	logfln("read dir: %d", fd);

	proc_t *p = cproc();
	file_handle *fh = &p->files[fd];
	vnode *dir = fh->vn;

	if (dir->read_dir == 0) {
		logfln("dir has not read_dir() method");
		SYSRET(-1);
	}

	int rc = dir->read_dir(dir->fn, fh->off, e);

	if (rc > -1)
		fh->off += rc;

	SYSRET(rc);
}

static void sys_exit_t(trapframe_t *tf) {
	logfln("exit thread");

	int is = enter_critical_section();

	proc_t *cp = current_proc();

	thread_t *ct = current_thread();
	ct->state = S_CLEANUP;

	if (!has_active_threads(cp)) {
		logfln("proc is out of threads, disposing");
		cp->state = S_CLEANUP;
	} else {
		logfln("proc has more threads");
	}

	exit_critical_section(is);

	reschedule();
}

static void sys_exit_p(trapframe_t *tf) {
	panic("exit process");
}

static void sys_wait(trapframe_t *tf) {
	SYSRET(-1);
}

static void sys_waitpid	(trapframe_t *tf) {

	int pid = ARG(tf, 0, int);
	logfln ("wait on pid: %d", pid);

	proc_t *p = get_proc(pid);

	while (p->state == S_USED);

	SYSRET(-1);
}


