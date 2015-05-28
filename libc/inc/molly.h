#pragma once

#include <string.h>

typedef struct {

	char 	name[256];

} dirent_t;

typedef enum {

	SEEK_SET,
	SEEK_CUR,
	SEEK_END

} seek_t;

enum { 

	SYS_LOG	,
	SYS_SBRK,
	SYS_MKT	,
	SYS_YIELD,
	SYS_FORK,
	SYS_YIELDP,
	SYS_STALL,
	SYS_EXEC,
	SYS_OPEN,
	SYS_EXIT,
	SYS_CLOSE,
	SYS_WRITE,
	SYS_READ,
	SYS_GET_CWD,
	SYS_READ_DIR,
	SYS_EXIT_T,
	SYS_EXIT_P

};

void 	log		(const char *msg);
void 	*sbrk	(int size);
int 	mkt		(int (*entry)());
void	yield	();
int		fork	();
void	yieldp	();
void	stall	(unsigned ms);
int		exec 	(char *path, int in, int out, int err);
int 	open	(char *path);
void 	exit	();
void 	exit_t	();
void 	exit_p	();
int 	open	(char *path);
int		close	(int fd);
int		write	(int fd, const void *buff, int count);
int		read	(int fd, const void *buff, int count);
size_t	getcwd 	(void *buff);
int		readdir (int fd, dirent_t *e);



