#pragma once

#include <types.h>

void initIDT();

void set_gate(word id, u32 isr);

