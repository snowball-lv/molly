#pragma once

#include <stdint.h>
#include <proc.h>

int load_image(char *bin, entry_f *e, uintptr_t *brk);