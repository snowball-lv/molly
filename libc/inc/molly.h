#pragma once

#include <string.h>

#define SYS_LOG			(1)
#define SYS_SBRK		(2)
#define SYS_MKT			(3)
#define SYS_YIELD		(4)
#define SYS_FORK		(5)
#define SYS_YIELDP		(6)
#define SYS_STALL		(7)
#define SYS_EXEC		(8)
#define SYS_OPEN		(9)
#define SYS_EXIT		(10)

void 	log		(const char *msg);
void 	*sbrk	(int size);
int 	mkt		(int (*entry)());
void	yield	();
int		fork	();
void	yieldp	();
void	stall	(unsigned ms);
int		exec	(char *path, char **args);
int 	open	(char *path);
void 	exit	();
















