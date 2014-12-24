#pragma once

#include <types.h>

#define LF	'\n'
#define CR	'\r'

void clear();
void printf(string format, ...);

#define printfln(args...) \
	(printf(args), printf("%c", LF))
	
	