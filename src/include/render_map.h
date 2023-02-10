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

/* $Id: render_map.h,v 1.1 2003/11/22 17:32:10 nsubtil Exp $ */

#ifndef _RENDER_MAP_H
#define _RENDER_MAP_H

extern void load_map_assets();

void map_setup_wall(struct game *game, int x, int y);
void map_lightmap_param_rocket(GLfloat params[4], struct game *game, int x, int y, int r);
void map_draw_vertical_wall(struct game *game, int x, int y);
void map_draw_horizontal_wall(struct game *game, int x, int y);
void map_draw_ll_wall(struct game *game, int x, int y);
void map_draw_lr_wall(struct game *game, int x, int y);
void map_draw_ur_wall(struct game *game, int x, int y);
void map_draw_ul_wall(struct game *game, int x, int y);
void map_draw_pacman_food(struct map *map, int x, int y);
void map_draw_pacman_pill(struct map *map, int x, int y);
void map_draw_wall(struct game *game, int x, int y);
void map_render_opaque_objects(struct game *game);
void map_draw_rocket_lightmap(struct game *game, int c);
void map_render_translucent_objects(struct game *game, int player_no);

#endif
