#include <vfs.h>
#include <types.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <kalloc.h>
#include <klib.h>
#include <devfs.h>

#define MAX_ENTRIES		16

#define S_FREE			1
#define S_USED			2

#define R_SUCCESS		1

typedef struct {

	int 	state;
	char 	*path;
	vnode 	*vn;

} mount_point;

#define MAX_MOUNT_POINTS	16

static mount_point mount_points[MAX_MOUNT_POINTS];

static mount_point *get_mp(char *path) {

	for (int i = 0; i < MAX_MOUNT_POINTS; i++) {

		mount_point *mp = &mount_points[i];

		if (mp->state == S_FREE)
			continue;

		//support only devfs for now
		if (strcmp(mp->path, "#") == 0) {
			if (path[0] == '#')
				return mp;
		}
	}

	return 0;
}

static mount_point *get_mp_slot() {

	for (int i = 0; i < MAX_MOUNT_POINTS; i++) {

		mount_point *mp = &mount_points[i];

		if (mp->state == S_FREE)
			return mp;
	}

	return 0;
}

typedef struct {

	int 	state;
	char 	*path;
	vnode 	vn;

} file_table_entry;

#define MAX_FILE_TABLE_ENTRIES	16

static file_table_entry file_table[MAX_FILE_TABLE_ENTRIES];

void init_vfs() {

	for (int i = 0; i < MAX_MOUNT_POINTS; i++)
		mount_points[i].state = S_FREE;

	for (int i = 0; i < MAX_FILE_TABLE_ENTRIES; i++)
		file_table[i].state = S_FREE;
}

static file_table_entry *get_empty_fte() {

	for (int i = 0; i < MAX_FILE_TABLE_ENTRIES; i++) {

		file_table_entry *fte = &file_table[i];

		if (fte->state == S_FREE)
				return fte;
	}

	return 0;
}

//absolute paths only
vnode *vfs_open(char *path) { 
	kprintfln("vfs open: [%s]", path);

	mount_point *mp = get_mp(path);

	if (mp == 0)
		panic("no mount point for: %s", path);

	file_table_entry *fte = get_empty_fte();

	if (fte == 0)
		panic("out of file table slots");

	char *rem = path_rem(path, mp->path);
	kprintfln("rem: %s", rem);
	int r = mp->vn->open(&fte->vn, rem);
	
	return 0;
}

void vfs_mount(char *path, vnode *vn) {

	mount_point *mp = get_mp_slot();

	if (mp == 0)
		panic("out of mount point slots");

	mp->state 	= S_USED;
	mp->path 	= kstrdup(path);
	mp->vn 		= vn;
}



















