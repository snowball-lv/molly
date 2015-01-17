#pragma once

#include <types.h>

#define LF	'\n'
#define CR	'\r'
#define TAB	'\t'

void clear();
void console_clear();
void printf(const byte *format, ...);
void kprintf(const byte *format, ...);

#define printfln(args...) \
	(printf(args), printf("%c", LF))

#define kprintfln(args...) \
	(kprintf(args), kprintf("%c", LF))
	
	