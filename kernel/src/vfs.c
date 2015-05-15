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
	vnode 	*fs;

} mount_point;

typedef struct {

	int 	state;
	char 	*path;
	vnode 	*vn;

} file_table_entry;

#define MAX_MOUNT_POINTS		16
#define MAX_FILE_TABLE_ENTRIES	16

static mount_point 		mount_points 	[MAX_MOUNT_POINTS];
static file_table_entry file_table 		[MAX_FILE_TABLE_ENTRIES];

void init_vfs() {

	for (int i = 0; i < MAX_MOUNT_POINTS; i++)
		mount_points[i].state = S_FREE;

	for (int i = 0; i < MAX_FILE_TABLE_ENTRIES; i++)
		file_table[i].state = S_FREE;
}

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

static file_table_entry *get_empty_fte() {

	for (int i = 0; i < MAX_FILE_TABLE_ENTRIES; i++) {

		file_table_entry *fte = &file_table[i];

		if (fte->state == S_FREE)
			return fte;
	}

	return 0;
}

static file_table_entry *look_in_ft(char *path) {

	for (int i = 0; i < MAX_FILE_TABLE_ENTRIES; i++) {

		file_table_entry *fte = &file_table[i];

		if (fte->state == S_FREE)
			continue;

		if (strcmp(fte->path, path) == 0)
			return fte;
	}

	return 0;
}

//absolute paths only
vnode *vfs_open(char *path) { 
	kprintfln("vfs open: [%s]", path);

	//add a search in ft

	mount_point *mp = get_mp(path);

	if (mp == 0)
		panic("no mount point for: %s", path);

	file_table_entry *fte = get_empty_fte();

	if (fte == 0)
		panic("out of file table slots");

	//reserve this fte
	fte->state = S_USED;

	char *rem = path_rem(path, mp->path);
	kprintfln("rem: %s", rem);

	vnode *vn = mp->fs->get_vnode(rem);

	if (vn == 0)
		panic("no node for: [%s]", path);

	if (vn->open(vn) == 1) {

		fte->vn 	= vn;
		fte->path 	= kstrdup(path);

		return vn;
		
	} else {

		panic("couldn't open file: [%s]", path);

	}

	//release fte
	fte->state = S_FREE;
	
	return 0;
}

static file_table_entry *find_fte(vnode *vn) {

	for (int i = 0; i < MAX_FILE_TABLE_ENTRIES; i++) {

		file_table_entry *fte = &file_table[i];

		if (fte->state == S_FREE)
			continue;

		if (fte->vn == vn)
			return fte;
	}

	return 0;
}

void vfs_close(vnode *vn) {
	kprintfln("vfs close");

	file_table_entry *fte = find_fte(vn);

	if (fte == 0)
		panic("vnode not in file table");

	kprintfln("closing: [%s]", fte->path);

	vn->close(vn->fn);

	kfree(fte->path);
	fte->state = S_FREE;
}

int vfs_write(vnode *vn, void *buff, size_t off, int count) {

	int r = vn->write(vn->fn, buff, off, count);

	return r;
}

int vfs_read(vnode *vn, void *buff, size_t off, int count) {

	int r = vn->read(vn->fn, buff, off, count);

	return r;
}

void vfs_mount(char *path, vnode *fs) {

	mount_point *mp = get_mp_slot();

	if (mp == 0)
		panic("out of mount point slots");

	mp->state 	= S_USED;
	mp->path 	= kstrdup(path);
	mp->fs 		= fs;
}

















