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
  "$Id: gfx.c,v 1.11 2003/11/22 17:32:09 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked-lists.h"
#include "file.h"
#include "gfx.h"

struct image_rgba32 *images = NULL;

/*
  lê um ficheiro TGA não-comprimido de 32bpp
  XXX - código horripilante
*/
struct image_rgba32 *gfx_read_targa(char *fname)
{
	FILE *fp;
	struct raw_targa tga;
	int c;
	unsigned char *buf, sb;
	struct image_rgba32 *bmp;
  
	bmp = (struct image_rgba32 *)malloc(sizeof(struct image_rgba32));
	if(bmp == NULL)
	{
		perror("malloc()");
		exit(1);
	}
	memset(bmp, 0, sizeof(struct image_rgba32));
	memset(&tga, 0, sizeof(struct raw_targa));
  
	if((int)(fp=file_open(fname)) == 0) return NULL;
	fread(&tga.id_len, sizeof(tga.id_len), 1, fp);
	fread(&tga.cl_map, sizeof(tga.cl_map), 1, fp);
  
	if(tga.cl_map==1)
	{
		fprintf(stderr, "read_targa(): color mapped images not supported\n");
		fclose(fp);
		return NULL;
	}

	fseek(fp, 12, SEEK_SET);
	fread(&tga.width, sizeof(tga.width), 1, fp);
	fread(&tga.height, sizeof(tga.height), 1, fp);
	fread(&tga.bpp, sizeof(tga.bpp), 1, fp);
	fseek(fp, tga.id_len+1, SEEK_CUR);
/*  
    bmp->x = tga.width;
    bmp->y = tga.height;
    bmp->bpp = (tga.bpp == 24 ? 32 : tga.bpp);
*/

	bmp->width = tga.width;
	bmp->height = tga.height;
  
	bmp->data=malloc(bmp->width*bmp->height*32/8);
/*  bmp->data_32 = bmp->data;
    bmp->data_16 = bmp->data;
    bmp->data_8 = bmp->data;*/

	if(bmp->data==NULL)
	{
		perror("malloc()");
		exit(1);
	}
	buf=(unsigned char *)bmp->data;
  
	if(tga.bpp==32 || tga.bpp==16)
	{
		int r;
      
		r = ftell(fp);
		r = fread(bmp->data, 1, 32/8*bmp->width*bmp->height, fp);
		fclose(fp);
	}
	else 
		for(c=0;c<bmp->width*bmp->height;c++)
		{
			fread(&buf[c*4], 3, 1, fp);
			buf[c*4+3]=0;
		}
/*
  if(bmp->bpp==24)
  {
  bmp->bpp=32;
  }
*/

	/*
	  for(c=0;c<bmp->x*bmp->y;c++)
	  {
	  switch(bmp->bpp)
	  {
	  case 32:
	  case 24:
	  sb = bmp->data_32[c] & 0xFF000000;
	  bmp->data_32[c] = bmp->data_32[c] ^ (sb << 24) + bmp->data_32[c] & 0xFFFFFF;
	  bmp->data_32[c] |= (bmp->data_32[c] & 0xFF) << 24;
	  bmp->data_32[c] = bmp->data_32[c] ^ (bmp->data_32[c] & 0xFF) + bmp->data_32[c] & 0xFFFFFF00;
	  bmp->data_32[c] |= sb;
	  break;
	  }
	  }
	*/
  
	for(c=0;c<bmp->width*bmp->height;c++)
	{
		switch(32)
		{
		case 32:
			sb=buf[c*4];
			buf[c*4]=buf[c*4+2];
			buf[c*4+2]=sb;
			break;
		case 24:
			sb=buf[c*3];
			buf[c*3]=buf[c*3+2];
			buf[c*3+2]=sb;
			break;
		}
	}

	bmp->name = strdup(fname);
	bmp->id = -1;
	bmp->filtered = 0;
	bmp->next = NULL;
	
	return bmp;
}

/*
  procura um bitmap já carregado
*/
struct image_rgba32 *gfx_find(char *name)
{
	struct image_rgba32 *cur;
	
	cur = images;
	while(cur)
	{
		if(strcmp(cur->name, name) == 0)
			return cur;
		
		cur = cur->next;
	}
	
	return NULL;
}

/*
  devolve um bitmap, carregando do ficheiro se necessário
*/
struct image_rgba32 *gfx_get(char *name)
{
	struct image_rgba32 *ret;
	
	ret = gfx_find(name);
	if(ret == NULL)
	{
		ret = gfx_read_targa(name);
		LLIST_ADD(struct image_rgba32, images, ret);
	}
	
	return ret;
}

/*
  altera o canal alpha da textura para a intensidade da cor em cada pixel
*/
void gfx_alpha_from_intensity(char *name)
{
	int x, y;
	struct image_rgba32 *img;
	
	img = gfx_get(name);
	if(img == NULL)
		return;
	
	for(x = 0; x < img->width; x++)
		for(y = 0; y < img->height; y++)
		{
			unsigned char *ptr;
			
			ptr = (unsigned char *)((unsigned long)img->data + (x + y * img->width) * 4);

			ptr[2] = (ptr[0] + ptr[1] + ptr[2]) / 3;
		}
}

/*
  altera o canal alpha da textura para transparente em pixels de uma determinada cor
*/
void gfx_alpha_from_key(char *name, unsigned char r, unsigned char g, unsigned char b)
{
	int c;
	struct image_rgba32 *img;
	unsigned char *ptr;
	
	img = gfx_get(name);
	if(img == NULL)
		return;

	ptr = (unsigned char *)img->data;
	
	for(c = 0; c < img->width * img->height; c++)
	{
		if(ptr[0] == r && ptr[1] == g && ptr[2] == b)
			ptr[3] = 0;
		else
			ptr[3] = 0xff;
		
		ptr += 4;
	}
}

/*
  adiciona uma textura à lista
*/
void gfx_add(struct image_rgba32 *new)
{
	if(gfx_find(new->name))
	{
		printf("gfx_add: bitmap repetido: %s\n", new->name);
		return;
	}
	
	LLIST_ADD(struct image_rgba32, images, new);
}

/*
  carrega uma textura na placa gráfica
*/
GLuint gfx_upload_texture(char *name)
{
	struct image_rgba32 *texture;
	GLuint id;
	
	texture = gfx_get(name);
	if(texture == NULL)
	{
		printf("gfx_upload_texture: %s não existe\n", name);
		return -1;
	}

	texture->filtered = 1;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture->width, texture->height, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);

	texture->id = id;
	return id;
}

/*
  carrega uma textura sem filtragem & afins
*/
GLuint gfx_upload_texture_nofilter(char *name)
{
	struct image_rgba32 *texture;
	GLuint id;

	texture = gfx_get(name);
	if(texture == NULL)
	{
		printf("gfx_upload_texture_no_filter: %s não existe\n", name);
		return -1;
	}

	texture->filtered = 0;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height,
		     0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
	
	texture->id = id;
	return id;
}

/*
  liberta todas as texturas na placa gráfica
*/
void gfx_unload_all(void)
{
	struct image_rgba32 *cur;

	cur = images;
	
	while(cur)
	{
		if(cur->id != -1)
		{
			glDeleteTextures(1, &cur->id);
			cur->id = -1;
		}
		
		cur = cur->next;
	}
}

/*
  envia todas as texturas para a placa gráfica
*/
void gfx_reload_all(void)
{
	struct image_rgba32 *cur;
	
	cur = images;
	while(cur)
	{
		if(cur->filtered)
			gfx_upload_texture(cur->name);
		else
			gfx_upload_texture_nofilter(cur->name);

		cur = cur->next;
	}
}

