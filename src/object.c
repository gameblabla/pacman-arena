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

static const char cvsid[] =
  "$Id: object.c,v 1.26 2003/11/23 00:06:39 nsubtil Exp $";

#include "common.h" // Default headers

#include "linked-lists.h"
#include "object.h"
#include "game.h"
#include "map.h"
#include "game.h"
#include "screen.h"
#include "render.h"
#include "include/file.h"

struct ocache_entry *object_cache = NULL;

int object_hash_vertex(float vx[3])
{
	int ret = -5;
	
	if(vx[X] >= 0.0 && vx[Y] >= 0.0)
		ret = 1;
	
	if(vx[X] < 0.0 && vx[Y] >= 0.0)
		ret = 2;
	
	if(vx[X] < 0.0 && vx[Y] < 0.0)
		ret = 3;
	
	if(vx[X] >= 0.0 && vx[Y] < 0.0)
		ret = 4;
	
	if(vx[Z] >= 0.0)
		ret += 4;
	
	return ret - 1;
}

int object_hash_face(struct face_tri *face)
{
	int a, b, c;
	
	a = object_hash_vertex(face->a);
	b = object_hash_vertex(face->b);
	c = object_hash_vertex(face->c);
	
	return (a + b + c) / 3;
}

void object_add_face(struct object *obj, struct face_tri *face)
{
	obj->num_fc++;
	obj->fc_list = realloc(obj->fc_list, sizeof(struct face_tri *) * obj->num_fc);
	obj->fc_list[obj->num_fc - 1] = face;
}

/*
  procura um objecto na lista pelo nome do ficheiro correspondente
*/
struct object *object_cache_find_file(char *fname, int *n_objs)
{
	struct ocache_entry *cur;

	cur = object_cache;
	while(cur)
	{
		if(strcmp(cur->name, fname) == 0)
		{
			*n_objs = cur->n_objs;
			return cur->objs;
		}

		cur = cur->next;
	}

	return NULL;
}

void object_cache_add_file(char *fname, struct object *objs, int n_objs)
{
	struct ocache_entry *new;

	new = malloc(sizeof(struct ocache_entry));
	new->name = fname;
	new->objs = objs;
	new->n_objs = n_objs;
	new->next = NULL;
	
	LLIST_ADD(struct ocache_entry, object_cache, new);
}

int object_face_equal(struct face_tri *f1, struct face_tri *f2)
{
	if(memcmp(f1->a, f2->a, sizeof(float) * 3) != 0)
		return 0;
	
	if(memcmp(f1->b, f2->b, sizeof(float) * 3) != 0)
		return 0;
	
	if(memcmp(f1->c, f2->c, sizeof(float) * 3) != 0)
		return 0;
	
	if(memcmp(f1->na, f2->na, sizeof(GLfloat) * 3) != 0)
		return 0;
	
	if(memcmp(f1->nb, f2->nb, sizeof(GLfloat) * 3) != 0)
		return 0;
	
	if(memcmp(f1->nc, f2->nc, sizeof(GLfloat) * 3) != 0)
		return 0;
	
	if(f1->color_tag != f2->color_tag)
		return 0;

	return 1;
}

/* XXX - que coisa lenta! */
int object_get_vertex_index(float **vx_list, int num_vx, float vec[3])
{
	int c;

	for(c = 0; c < num_vx; c++)
		if(memcmp(vx_list[c], vec, sizeof(float) * 3) == 0)
			return c;
	
	return -1;
}

/* XXX - e este então nem se fala */
int object_get_face_index(struct face_tri **fc_list, int num_fc, struct face_tri *fc)
{
	int c;
	
	for(c = 0; c < num_fc; c++)
		if(object_face_equal(fc_list[c], fc))
			return c;
	
	return -1;
}

/*
  formato:
  num de vértices (4b)
  lista de vértices (3f cada, sem repetições)
  num de faces (4b)
  lista de faces (sem repetições):
    vértices a, b, c (índices na lista, 4b cada)
    normais na, nb, nc (3 floats cada)
    cor (4 floats)
    tag de cor (1 byte)
  num de objectos (4b)
  lista de objectos:
    num de faces do objecto
    lista de índices das faces do objecto na lista de faces (4b cada)

  XXX - isto é muito lento
*/
void object_write_file(char *fname, struct object *objs, int n_objs)
{
	FILE *fp;
	int c, d;
	float **vx_list[8];
	int vx_num[8], vx_tot;
	struct face_tri **fc_list[8];
	int fc_num[8], fc_tot;
	
	/* gerar lista de vértices */
	printf("Building vertex list... ");
	fflush(stdout);

	for(c = 0; c < 8; c++)
	{
		vx_list[c] = NULL;
		vx_num[c] = 0;
	}
	
	for(c = 0; c < n_objs; c++)
		for(d = 0; d < objs[c].num_fc; d++)
		{
			int h;
			
			h = object_hash_vertex(objs[c].fc_list[d]->a);
			if(object_get_vertex_index(vx_list[h], vx_num[h], objs[c].fc_list[d]->a) == -1)
			{
				vx_num[h]++;
				vx_list[h] = realloc(vx_list[h], vx_num[h] * sizeof(float *));
				vx_list[h][vx_num[h] - 1] = objs[c].fc_list[d]->a;
			}
			
			h = object_hash_vertex(objs[c].fc_list[d]->b);
			if(object_get_vertex_index(vx_list[h], vx_num[h], objs[c].fc_list[d]->b) == -1)
			{
				vx_num[h]++;
				vx_list[h] = realloc(vx_list[h], vx_num[h] * sizeof(float *));
				vx_list[h][vx_num[h] - 1] = objs[c].fc_list[d]->b;
			}

			h = object_hash_vertex(objs[c].fc_list[d]->c);
			if(object_get_vertex_index(vx_list[h], vx_num[h], objs[c].fc_list[d]->c) == -1)
			{
				vx_num[h]++;
				vx_list[h] = realloc(vx_list[h], vx_num[h] * sizeof(float *));
				vx_list[h][vx_num[h] - 1] = objs[c].fc_list[d]->c;
			}		
		}


	vx_tot = 0;
	for(c = 0; c < 8; c++)
		vx_tot += vx_num[c];
	
	printf("%d unique vertexes\n", vx_tot);

	/* gerar lista de faces */
	printf("Building face list... ");
	fflush(stdout);
	
	for(c = 0; c < 8; c++)
	{
		fc_list[c] = NULL;
		fc_num[c] = 0;
	}
	
	for(c = 0; c < n_objs; c++)
		for(d = 0; d < objs[c].num_fc; d++)
		{
			int h;
			
			h = object_hash_face(objs[c].fc_list[d]);
			if(object_get_face_index(fc_list[h], fc_num[h], objs[c].fc_list[d]) == -1)
			{
				/* face não existe na lista */
				fc_num[h]++;
				fc_list[h] = realloc(fc_list[h], fc_num[h] * sizeof(struct fc_tri *));
				fc_list[h][fc_num[h] - 1] = objs[c].fc_list[d];
			}
		}

	fc_tot = 0;
	for(c = 0; c < 8; c++)
		fc_tot += fc_num[c];
	
	printf("%d unique faces\n", fc_tot);

	fp = file_open(fname);
	if(fp == NULL)
	{
		printf("object_write_file: parece que %s não dá para a coisa\n", fname);
		exit(1);
	}

	
	/* escrever lista de vértices */
	printf("Writing vertex list...");
	fflush(stdout);

	file_write_values32(&vx_tot, 1, fp);
	for(d = 0; d < 8; d++)
		for(c = 0; c < vx_num[d]; c++)
			file_write_values32(vx_list[d][c], 3, fp);
	
	/* escrever lista de faces */
	printf("\nWriting face list...");
	fflush(stdout);

	file_write_values32(&fc_tot, 1, fp);
	for(c = 0; c < 8; c++)
		for(d = 0; d < fc_num[c]; d++)
		{
			int idx[3], h[3], e;
			unsigned char tag;
		
			h[0] = object_hash_vertex(fc_list[c][d]->a);
			idx[0] = object_get_vertex_index(vx_list[h[0]], vx_num[h[0]], fc_list[c][d]->a);
			h[1] = object_hash_vertex(fc_list[c][d]->b);
			idx[1] = object_get_vertex_index(vx_list[h[1]], vx_num[h[1]], fc_list[c][d]->b);
			h[2] = object_hash_vertex(fc_list[c][d]->c);
			idx[2] = object_get_vertex_index(vx_list[h[2]], vx_num[h[2]], fc_list[c][d]->c);
			
			if(idx[0] == -1 || idx[1] == -1 || idx[2] == -1)
			{
				printf("object_write_file: duh ?!\n");
				abort();
			}
			
			for(e = 0; e < h[0]; e++)
				idx[0] += vx_num[e];
			
			for(e = 0; e < h[1]; e++)
				idx[1] += vx_num[e];

			for(e = 0; e < h[2]; e++)
				idx[2] += vx_num[e];
		
			file_write_values32(idx, 3, fp);
			file_write_values32(fc_list[c][d]->na, 3, fp);
			file_write_values32(fc_list[c][d]->nb, 3, fp);
			file_write_values32(fc_list[c][d]->nc, 3, fp);
			file_write_values32(fc_list[c][d]->color, 4, fp);
		
			tag = fc_list[c][d]->color_tag & 0xff;
			fwrite(&tag, 1, 1, fp);
		}

	printf("\nWriting objects... ");
	fflush(stdout);

	/* escrever objectos */
	file_write_values32(&n_objs, 1, fp);
	for(c = 0; c < n_objs; c++)
	{
		file_write_values32(&objs[c].num_fc, 1, fp);
		for(d = 0; d < objs[c].num_fc; d++)
		{
			int i, h, e;
			
			h = object_hash_face(objs[c].fc_list[d]);
			i = object_get_face_index(fc_list[h], fc_num[h], objs[c].fc_list[d]);

			if(i == -1)
			{
				printf("object_write_file: duh-duh!\n");
				abort();
			}

			for(e = 0; e < h; e++)
				i += fc_num[e];
			
			file_write_values32(&i, 1, fp);
		}

		printf("%d ", c);
		fflush(stdout);
	}

	printf("\n");

	fclose(fp);

	for(c = 0; c < 8; c++)
	{
		free(vx_list[c]);
		free(fc_list[c]);
	}
}

struct object *object_read_file(char *fname, int *n_objs)
{
	FILE *fp;
	int c, d;
	float **vx_list;
	int vx_num;
	struct face_tri *fc_list;
	int fc_num;
	struct object *objs;

	objs = object_cache_find_file(fname, n_objs);

	if(objs)
		return objs;

	fp = file_open(fname);
	if(fp == NULL)
	{
		printf("object_read_file: adonde 'tá a porcaria do %s ?\n", fname);
		exit(1);
	}

	/* ler lista de vértices */
	file_read_values32(&vx_num, 1, fp);
	vx_list = malloc(sizeof(float *) * vx_num);
	for(c = 0; c < vx_num; c++)
	{
		vx_list[c] = malloc(sizeof(float) * 3);
		file_read_values32(vx_list[c], 3, fp);
	}

	/* ler lista de faces */
	file_read_values32(&fc_num, 1, fp);
	fc_list = malloc(sizeof(struct face_tri) * fc_num);
	for(c = 0; c < fc_num; c++)
	{
		int idx[3];
		unsigned char tag;
		
		file_read_values32(idx, 3, fp);
		fc_list[c].a = vx_list[idx[0]];
		fc_list[c].b = vx_list[idx[1]];
		fc_list[c].c = vx_list[idx[2]];
		
		file_read_values32(fc_list[c].na, 3, fp);
		file_read_values32(fc_list[c].nb, 3, fp);
		file_read_values32(fc_list[c].nc, 3, fp);
		
		file_read_values32(fc_list[c].color, 4, fp);
		
		fread(&tag, 1, 1, fp);
		fc_list[c].color_tag = (int)(tag & 0xff);
	}

	/* ler lista de objectos */
	file_read_values32(n_objs, 1, fp);
	printf("%s: %d objects (%d unique vertexes, %d unique faces)\n",
	       fname, *n_objs, vx_num, fc_num);
	objs = malloc(sizeof(struct object) * (*n_objs));
	for(c = 0; c < *n_objs; c++)
	{
		file_read_values32(&objs[c].num_fc, 1, fp);
		objs[c].fc_list = malloc(sizeof(struct face_tri *) * objs[c].num_fc);
		
		for(d = 0; d < objs[c].num_fc; d++)
		{
			int idx;
			struct face_tri *face;
			
			file_read_values32(&idx, 1, fp);
			face = &fc_list[idx];

			objs[c].fc_list[d] = face;
		}

		objs[c].dlist_color = -1;
		objs[c].dlist_nocolor = -1;

/* evitar linkar o jogo todo no 3dsconv */
#ifndef TDSCONV
		render_compile_dlist(&objs[c]);
#endif
	}

	fclose(fp);
	free(vx_list);

	object_cache_add_file(fname, objs, *n_objs);

	return objs;
}

#ifndef TDSCONV
/*
  liberta todas as display lists na placa gráfica
*/
void object_release_dlists(void)
{
	struct ocache_entry *cur;
	
	cur = object_cache;
	while(cur)
	{
		int c;

		for(c = 0; c < cur->n_objs; c++)
		{
			if(cur->objs[c].dlist_color != -1)
			{
				glDeleteLists(cur->objs[c].dlist_color, 1);
				cur->objs[c].dlist_color = -1;
			}
			
			if(cur->objs[c].dlist_nocolor != -1)
			{
				glDeleteLists(cur->objs[c].dlist_nocolor, 1);
				cur->objs[c].dlist_nocolor = -1;
			}
		}
		
		cur = cur->next;
	}
}

/*
  recompila display lists
*/
void object_recompile_dlists(void)
{
	struct ocache_entry *cur;
	
	cur = object_cache;
	while(cur)
	{
		int c;
		
		for(c = 0; c < cur->n_objs; c++)
			render_compile_dlist(&cur->objs[c]);
		
		cur = cur->next;
	}
}
#endif
