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

/* $Id: menu.h,v 1.2 2003/11/22 17:32:10 nsubtil Exp $ */

#ifndef _MENU_H
#define _MENU_H

struct menu_entry
{
	char *image_name_high, *image_name_low;
	struct image_rgba32 *image_high, *image_low;
};

void menu_setup_map(struct map *map);
void menu_run(void);
void menu_resolution(struct game *game);
int menu(struct game *game, struct menu_entry *m, int n);

#endif
