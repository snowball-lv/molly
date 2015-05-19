#pragma once

#include <console.h>

#ifdef _MOLLY_DEBUG_
	
	#define logf(args...) 	(kprintf(args))
	#define logfln(args...) (kprintf(args), kprintf("%c", LF))

#else

	#define logf(args...)
	#define logfln(args...)

#endif