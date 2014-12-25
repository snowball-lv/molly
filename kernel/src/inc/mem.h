#pragma once

#include <types.h>

#define MMAP_AVAILABLE 	0x1
#define MMAP_RESERVED	0x2
#define MMAP_RECLAIM	0x3
#define MMAP_NVS		0x4

#define ATTR __attribute__((packed))

typedef struct {
	u64 base;
	u64 size;
	u32 type;
	u32 reserved;
} ATTR MemMapEntry;

#undef ATTR

typedef struct {
	uword size;
	MemMapEntry *entries;
} MemMap;















