#pragma once

#include <types.h>

#define MMAP_AVAILABLE 	0x1
#define MMAP_RESERVED	0x2
#define MMAP_RECLAIM	0x3
#define MMAP_NVS		0x4

#define ATTR __attribute__((packed))

typedef struct {
	uint64_t base;
	uint64_t size;
	uint32_t type;
	uint32_t reserved;
} ATTR MemMapEntry;

#undef ATTR

typedef struct {
	uword size;
	MemMapEntry *entries;
} MemMap;