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

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "tds.h"

static const char cvsid[] = 
  "$Id: 3dsconv.c,v 1.16 2003/07/19 01:10:17 nsubtil Exp $";

void convert(char *format, char *out, int start, int end)
{
	char fname[100];
	int c;
	struct object *objects;
	
	objects = malloc(sizeof(struct object) * (end - start + 1));
	printf("%s (%d frames):\n", out, end - start + 1);

	for(c = 0; c < end - start + 1; c++)
	{
		struct object *tmp;
		
		sprintf(fname, format, c + start);
		tmp = tds_load(fname);
		memcpy(&objects[c], tmp, sizeof(struct object));
	}
	
	object_write_file(out, objects, end - start + 1);
	
	printf("\n");
}

int main(int argc, char **argv)
{
	convert("3ds/wall-vertical.3ds", "gfx/wall-vertical.3d", 0, 0);
	convert("3ds/wall-horizontal.3ds", "gfx/wall-horizontal.3d", 0, 0);
	convert("3ds/wall-ul.3ds", "gfx/wall-ul.3d", 0, 0);
	convert("3ds/wall-ur.3ds", "gfx/wall-ur.3d", 0, 0);
	convert("3ds/wall-lr.3ds", "gfx/wall-lr.3d", 0, 0);
	convert("3ds/wall-ll.3ds", "gfx/wall-ll.3d", 0, 0);
	convert("3ds/bomb.3ds", "gfx/bomb.3d", 0, 0);
	convert("3ds/rocket.3ds", "gfx/rocket.3d", 0, 0);

	convert("3ds/pacman-moving-%d.3ds", "gfx/pacman-moving.3d", 0, 29);
	convert("3ds/pacman-stopped-%d.3ds", "gfx/pacman-stopped.3d", 30, 59);
	convert("3ds/pacman-dying-%d.3ds", "gfx/pacman-dying.3d", 60, 99);
	convert("3ds/pacman-jumping-%d.3ds", "gfx/pacman-jumping.3d", 101, 130);
	convert("3ds/ghost-green-moving-%d.3ds", "gfx/ghost-green-moving.3d", 0, 29);
	convert("3ds/ghost-green-dying-%d.3ds", "gfx/ghost-green-dying.3d", 30, 64);
	convert("3ds/ghost-green-returning-%d.3ds", "gfx/ghost-green-returning.3d", 65, 90);

	return 0;
}
