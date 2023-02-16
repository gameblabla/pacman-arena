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

#include "common.h" // Default headers

/* lib3ds */
#include <file.h>
#include <mesh.h>
#include <node.h>
#include <material.h>
#include <vector.h>

#include "object.h"
#include "linked-lists.h"
#include "tds.h"

static const char cvsid[] = 
  "$Id: tds.c,v 1.9 2003/07/19 01:10:18 nsubtil Exp $";

static struct tdsfile *tdsfiles = NULL;

/*
  carrega um ficheiro 3ds
*/
struct tdsfile *tds_load_file(char *name)
{
	Lib3dsFile *h;
	struct tdsfile *new;

	h = lib3ds_file_load(name);
	if(h == NULL)
	{
		printf("%s: tds_load_file: lib3ds_file_load() failed\n", name);
		return NULL;
	}
	
	new = (struct tdsfile *)malloc(sizeof(struct tdsfile));
	new->name = strdup(name);
	new->handler = h;
	new->next = NULL;
	
	LLIST_ADD(struct tdsfile, tdsfiles, new);
	return new;
}

/*
  procura um ficheiro 3ds carregado
*/
struct tdsfile *tds_find(char *name)
{
	struct tdsfile *cur;
	
	cur = tdsfiles;
	while(cur)
	{
		if(strcmp(cur->name, name) == 0)
			return cur;
		
		cur = cur->next;
	}
	
	return NULL;
}

struct tdsfile *tds_get(char *name)
{
	struct tdsfile *ret;
	
	ret = tds_find(name);
	if(ret == NULL)
		ret = tds_load_file(name);
	
	return ret;
}

/*
  carrega o mesh de um 3ds
*/
struct object *tds_load(char *name)
{
	struct tdsfile *file;
	struct object *ret;
	Lib3dsNode *p;
	
	file = tds_get(name);
	if(file == NULL)
		return NULL;
	
	lib3ds_file_eval(file->handler, 0.0);
	ret = malloc(sizeof(struct object));
	ret->num_fc = 0;
	ret->fc_list = NULL;
	ret->dlist_color = -1;
	ret->dlist_nocolor = -1;

	p = file->handler->nodes;
	while(p)
	{
		tds_add_node_recursive(ret, p, file);
		p = p->next;
	}

	return ret;
}

void tds_add_node_recursive(struct object *obj, Lib3dsNode *node, struct tdsfile *file)
{
	Lib3dsNode *p;
	Lib3dsMesh *mesh;
	Lib3dsVector *normal_list;
	int c;
	int flag;

	p = node->childs;
	while(p)
	{
		tds_add_node_recursive(obj, p, file);
		p = p->next;
	}

	if(node->type != LIB3DS_OBJECT_NODE)
		return;
	
	/* XXX - que raio é isto ? */
	if(strcmp(node->name, "$$$DUMMY") == 0)
		return;

	if(strstr(node->name, "ctag"))
		flag = 1;
	else
		flag = 0;

	mesh = lib3ds_file_mesh_by_name(file->handler, node->name);
	if(mesh == NULL)
	{
		printf("tds_add_node_recursive: in [%s]: mesh is null!\n", node->name);
		return;
	}

	normal_list = (Lib3dsVector *)malloc(sizeof(Lib3dsVector) * mesh->faces * 3);
	lib3ds_mesh_calculate_normals(mesh, normal_list);

	for(c = 0; c < mesh->faces; c++)
	{
		Lib3dsFace *face;
		struct face_tri *new;

		new = malloc(sizeof(struct face_tri));

		face = &mesh->faceL[c];
		
		if(face->material[0])
		{
			Lib3dsMaterial *material;

			material = lib3ds_file_material_by_name(file->handler, face->material);
			memcpy(new->color, material->diffuse, sizeof(GLfloat) * 4);
		} else {
			new->color[R] = 1.0;
			new->color[G] = 1.0;
			new->color[B] = 1.0;
			new->color[A] = 1.0;
		}

		new->a = mesh->pointL[face->points[0]].pos;
		new->b = mesh->pointL[face->points[1]].pos;
		new->c = mesh->pointL[face->points[2]].pos;		

		memcpy(new->na, normal_list[3 * c], sizeof(GLfloat) * 3);
		memcpy(new->nb, normal_list[3 * c + 1], sizeof(GLfloat) * 3);
		memcpy(new->nc, normal_list[3 * c + 2], sizeof(GLfloat) * 3);

		new->color_tag = flag;
		
		object_add_face(obj, new);
	}

	free(normal_list);
}
