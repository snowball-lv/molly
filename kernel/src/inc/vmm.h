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
















