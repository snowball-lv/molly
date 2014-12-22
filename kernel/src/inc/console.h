#pragma once

#include <types.h>

#define NL	'\n'

void clear();
void printf(string format, ...);

#define printfln(args...) \
	(printf(args), printf("%c", NL))
	
	