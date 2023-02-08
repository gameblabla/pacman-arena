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
  "$Id: file.c,v 1.5 2003/11/23 00:06:39 nsubtil Exp $";

#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/file.h"

FILE *file_open(char *file)
{
	char fname[FILE_NAME_LEN];

	if(strlen(file) + strlen(FILE_BASE_PATH) + 2 > FILE_NAME_LEN)
	{
		printf("file_open: %s: that's too much for me\n", file);
		exit(1);
	}
	
	sprintf(fname, "%s/%s", FILE_BASE_PATH, file);
	return fopen(fname, "rb");
}

/*
  all data on files is in little-endian format
*/
void file_read_values32(void *ret, int n, FILE *fp)
{
#ifdef SDL_LIL_ENDIAN
	fread(ret, 4, n, fp);
#else
	int c, *i;

	fread(ret, 4, n, fp);
	i = (int *)ret;
	
	for(c = 0; c < n; c++)
		i[c] = SDL_Swap32(i[c]);
#endif /* SDL_LIL_ENDIAN */
}

void file_write_values32(void *src, int n, FILE *fp)
{
#ifdef SDL_LIL_ENDIAN
	fwrite(src, 4, n, fp);
#else
	int c, tmp, *i;
	
	i = (int *)src;
	for(c = 0; c < n; c++)
	{
		tmp = SDL_Swap32(i[c]);
		fwrite(&tmp, 4, 1, fp);
	}
#endif /* SDL_LIL_ENDIAN */
}

char *file_make_fname(char *file)
{
	static char fname[FILE_NAME_LEN];

	if(strlen(FILE_BASE_PATH) + strlen(file) + 2 > FILE_NAME_LEN)
	{
		printf("file_make_fname: too much!\n");
		exit(1);
	}

	sprintf(fname, "%s/%s", FILE_BASE_PATH, file);
	return fname;
}
