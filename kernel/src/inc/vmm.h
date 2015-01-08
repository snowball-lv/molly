#pragma once

#include<types.h>

#define PAGE_SIZE			4096

void initVMM();

void *kalloc(size_t size);
void kfree(void *ptr);












