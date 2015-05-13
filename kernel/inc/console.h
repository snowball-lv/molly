#pragma once

#define LF	'\n'
#define CR	'\r'
#define TAB	'\t'

void console_clear();
void kprintf(const char *format, ...);

#define kprintfln(args...) \
	(kprintf(args), kprintf("%c", LF))
	
void init_console();