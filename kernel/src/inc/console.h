#pragma once

#include <types.h>

#define LF	'\n'
#define CR	'\r'
#define TAB	'\t'

void clear();
void printf(string format, ...);

#define printfln(args...) \
	(printf(args), printf("%c", LF))
	
	