/*
  Pacman Arena
  Copyright (C) 2003 Nuno Subtil

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/* $Id: object.h,v 1.2 2003/11/22 17:32:10 nsubtil Exp $ */

#ifndef _OBJECT_H
#define _OBJECT_H

#define X 0
#define Y 1
#define Z 2
#define W 3

#define R 0
#define G 1
#define B 2
#define A 3

struct face_tri
{
	/* vértices */
	float *a, *b, *c;
	/* normais */
	float na[3], nb[3], nc[3];
	
	/* cor da face */
	float color[4];
	/* tag de cor */
	int color_tag;
};

struct object
{
	/* num de faces */
	int num_fc;
	/* lista de faces */
	struct face_tri **fc_list;

	/* display list deste objecto para polígonos de cor fixa*/
	GLuint dlist_color;
	/* polígonos de cor variável */
	GLuint dlist_nocolor;

	struct object *next;
};

/* caches */
struct ocache_entry
{
	char *name;
	struct object *objs;
	int n_objs;

	struct ocache_entry *next;
};

int object_hash_vertex(float vx[3]);
int object_hash_face(struct face_tri *face);
void object_add_face(struct object *obj, struct face_tri *face);
struct object *object_cache_find_file(char *fname, int *n_objs);
void object_cache_add_file(char *fname, struct object *objs, int n_objs);
int object_face_equal(struct face_tri *f1, struct face_tri *f2);
int object_get_vertex_index(float **vx_list, int num_vx, float vec[3]);
int object_get_face_index(struct face_tri **fc_list, int num_fc, struct face_tri *fc);
void object_write_file(char *fname, struct object *objs, int n_objs);
struct object *object_read_file(char *fname, int *n_objs);
void object_release_dlists(void);
void object_recompile_dlists(void);

#endif
