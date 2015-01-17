#pragma once

#include <types.h>

#define LF	'\n'
#define CR	'\r'
#define TAB	'\t'

void console_clear();
void kprintf(const byte *format, ...);

#define kprintfln(args...) \
	(kprintf(args), kprintf("%c", LF))
	
	