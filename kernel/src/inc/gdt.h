#pragma once

#define K_CODE_SEL (0x8)

void init_gdt();

//gdt_asm.asm
void user_mode();

void set_tss(void *esp0);