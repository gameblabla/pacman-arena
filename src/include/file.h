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

/* $Id: file.h,v 1.2 2003/11/30 19:54:38 nsubtil Exp $ */

#ifndef _FILE_H
#define _FILE_H

#include <stdio.h>
#include <stdlib.h>

#ifndef DATADIR
#define FILE_BASE_PATH "."
#else
#define FILE_BASE_PATH DATADIR
#endif

#define FILE_NAME_LEN 100

FILE *file_open(char *file);
void file_read_values32(void *ret, int n, FILE *fp);
void file_write_values32(void *src, int n, FILE *fp);
char *file_make_fname(char *file);

#endif
