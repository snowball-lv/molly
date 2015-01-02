#pragma once

#include<types.h>

void initVMM();

typedef u32 PTEntry;

typedef struct {
	PTEntry entries[1024];
} PTable;

typedef u32 PDEntry;

typedef struct {
	PDEntry entries[1024];
} PDirectory;

PDirectory *vmm_get_dir();

#define PT_INDEX(x)	(((x) >> 12) & 0x3ff)
#define PD_INDEX(x)	(((x) >> 22) & 0x3ff)






















