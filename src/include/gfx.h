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

/* $Id: gfx.h,v 1.2 2003/11/22 17:32:10 nsubtil Exp $ */

#ifndef _GFX_H
#define _GFX_H

struct image_rgba32
{
	char *name;
	GLuint id;
	int filtered;

	int width, height;
	int *data;

	struct image_rgba32 *next;
};

struct raw_targa {
	unsigned char id_len;		// id field length
	unsigned char cl_map;		// color map type (0=no color map)
	unsigned char img_type;		// image type code (we want 2 for unmapped rgb)

	/* color map specification, shouldn't be needed */
	unsigned short clmap_orig;
	unsigned short clmap_len;
	unsigned char clmap_esize;
	/* end color map specification */
	
	/* image specification */
	unsigned short x_orig;		// x origin of the image
	unsigned short y_orig;		// y origin of the image
	unsigned short width;		// width of the image
	unsigned short height;		// height of the image
	unsigned char bpp;		// bits per pixel
	unsigned char img_desc_flag;	// bits 0-3: alpha channel bit number
					// bit 4: reserved (0)
					// bit 5: screen origin (0=lower left, 1=upper left)
					// bits 6-7: interleaving flag (hopefully 0)
	unsigned char img_desc[256];
	void *data;			// BGR[A] ordering
};

struct image_rgba32 *gfx_read_targa(char *fname);
struct image_rgba32 *gfx_find(char *name);
struct image_rgba32 *gfx_get(char *name);
void gfx_alpha_from_intensity(char *name);
void gfx_alpha_from_key(char *name, unsigned char r, unsigned char g, unsigned char b);
void gfx_add(struct image_rgba32 *new);
GLuint gfx_upload_texture(char *name);
GLuint gfx_upload_texture_nofilter(char *name);
void gfx_unload_all(void);
void gfx_reload_all(void);

#endif
