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
  "$Id: map.c,v 1.47 2003/11/22 17:32:09 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "object.h"
#include "game.h"
#include "player.h"
#include "render.h"
#include "gfx.h"
#include "m_math.h"
#include "shot.h"

#include "map.h"

struct map *map_load_from_ascii(char **game_map, char **ghost_map, int width, int height)
{
	struct map *ret;
	int x, y;
	
	ret = (struct map *)malloc(sizeof(struct map));
	ret->width = width;
	ret->height = height;

	ret->tiles = (struct map_tile **)malloc(sizeof(struct map_tile *) * height);
	for(y = 0; y < height; y++)
	{
		ret->tiles[y] = malloc(strlen(game_map[y]) * sizeof(struct map_tile));
		
		for(x = 0; x < width ; x++)
		{
			switch(game_map[height - 1 - y][x])
			{
			case 'L':
				MAP(ret, x, y).wall = MAP_WALL_UL;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			case 'l':
				MAP(ret, x, y).wall = MAP_WALL_LL;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			case 'R':
				MAP(ret, x, y).wall = MAP_WALL_UR;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			case 'r':
				MAP(ret, x, y).wall = MAP_WALL_LR;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			case '-':
				MAP(ret, x, y).wall = MAP_WALL_HORIZONTAL;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			case '|':
				MAP(ret, x, y).wall = MAP_WALL_VERTICAL;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			case 'N':
				MAP(ret, x, y).wall = MAP_WALL_GHOST_ONLY;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;

			case 'U':
				MAP(ret, x, y).wall = MAP_WALL_GHOST_ONLY;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_UP;
				break;

			case '1':
				MAP(ret, x, y).wall = MAP_WALL_NONE;
				MAP(ret, x, y).content = MAP_CONTENT_TELEPORT;
				/* XXX - ugly, terrible hack, don't look! */
				MAP(ret, x, y).c_data.teleport.dest_x = 0;
				MAP(ret, x, y).c_data.teleport.dest_y = y;
				MAP_TELEPORT(ret, x, y).direction = DIRECTION_RIGHT;
				/* it is safe to look again now */
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_LEFT;
				break;
				
			case '0':
				MAP(ret, x, y).wall = MAP_WALL_NONE;
				MAP(ret, x, y).content = MAP_CONTENT_TELEPORT;
				/* XXX - close your eyes... */
				MAP(ret, x, y).c_data.teleport.dest_x = ret->width - 1;
				MAP(ret, x, y).c_data.teleport.dest_y = y;
				MAP_TELEPORT(ret, x, y).direction = DIRECTION_LEFT;

				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_RIGHT;
				break;

			case '.':
				MAP(ret, x, y).wall = MAP_WALL_NONE;
				MAP(ret, x, y).content = MAP_CONTENT_FOOD;
				MAP(ret, x, y).c_data.food.type = FOOD_TYPE_NORMAL;
				MAP(ret, x, y).c_data.food.status = FOOD_STATUS_ACTIVE;
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			case '*':
				MAP(ret, x, y).wall = MAP_WALL_NONE;
				MAP(ret, x, y).content = MAP_CONTENT_PILL;
				MAP(ret, x, y).c_data.pill.status = PILL_STATUS_ACTIVE;
				MAP(ret, x, y).c_data.pill.phase = 
					2.0 * M_PI * (float)((rand() % 10) / 9.0);
				MAP(ret, x, y).flags = MAP_FLAGS_NONE;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			case 'G':
				MAP(ret, x, y).wall = MAP_WALL_GHOST_ONLY;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAG_GHOST_START_POSITION;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			case 'S':
				MAP(ret, x, y).wall = MAP_WALL_NONE;
				MAP(ret, x, y).content = MAP_CONTENT_NONE;
				MAP(ret, x, y).flags = MAP_FLAG_PACMAN_START_POSITION;
				MAP(ret, x, y).ghost_dir_alive = DIRECTION_NONE;
				break;
				
			default:
				printf("map_load_from_ascii: ora portantos temos aqui um %d... (y %d, x %d, w %d, h %d)\n", game_map[height - 1 - y][x], y, x, width, height);
				exit(1);
				break;
			}

			if(ghost_map == NULL)
				MAP(ret, x, y).ghost_dir = DIRECTION_NONE;
			else
				switch(ghost_map[height - 1 - y][x])
				{
				case 'U':
					MAP(ret, x, y).ghost_dir = DIRECTION_UP;
					break;
				
				case 'D':
					MAP(ret, x, y).ghost_dir = DIRECTION_DOWN;
					break;
				
				case 'L':
					MAP(ret, x, y).ghost_dir = DIRECTION_LEFT;
					break;
				
				case 'R':
					MAP(ret, x, y).ghost_dir = DIRECTION_RIGHT;
					break;
				
				default:
					MAP(ret, x, y).ghost_dir = DIRECTION_NONE;
					break;
				}
		}
	}
	
	return ret;
}

void map_free(struct map *map)
{
	int c;
	
	for(c = 0; c < map->height; c++)
		free(map->tiles[c]);
	
	free(map->tiles);
	free(map);
}

void map_update(struct map *map, float delta)
{
	int x, y;
	
	for(x = 0; x < map->width; x++)
		for(y = 0; y < map->height; y++)
		{
			switch(MAP(map, x, y).content)
			{
			case MAP_CONTENT_FOOD:
				/* update food */
				switch(MAP_FOOD(map, x, y).type)
				{
				case FOOD_TYPE_RESPAWN:
					if(MAP_FOOD(map, x, y).status == FOOD_STATUS_EATEN)
					{
						MAP_FOOD(map, x, y).time -= delta;
						if(MAP_FOOD(map, x, y).time <= 0.0)
							MAP_FOOD(map, x, y).status =
								FOOD_STATUS_ACTIVE;
					}

					break;

				case FOOD_TYPE_INTERMITTENT:
					MAP_FOOD(map, x, y).time -= delta;
					if(MAP_FOOD(map, x, y).time <= 0.0)
					{
						switch(MAP_FOOD(map, x, y).status)
						{
						case FOOD_STATUS_ACTIVE:
							MAP_FOOD(map, x, y).status =
								FOOD_STATUS_EATEN;
							MAP_FOOD(map, x, y).time =
								MAP_FOOD(map, x, y).inactive_time;
							break;
							
						case FOOD_STATUS_EATEN:
							MAP_FOOD(map, x, y).status =
								FOOD_STATUS_ACTIVE;
							MAP_FOOD(map, x, y).time =
								MAP_FOOD(map, x, y).active_time;
							break;
						}
					}
				}
			}
		}
}

int map_collision_square(struct game *game, int x, int y)
{
	struct map *map;
	
	map = game->map;
	
	if(MAP(map, x, y).wall == MAP_WALL_NONE ||
	   MAP(map, x, y).wall == MAP_WALL_GHOST_ONLY)
		return 0;
	
	return 1;
}

/*
  detects collisions on the map along a line between two points
  returns 0 if no collision is detected or 1 with the tile where collision occurs in ret
*/
int map_detect_collision(struct game *game, float start_pos[3], float end_pos[3], int ret[2])
{
	/* bresenham line algorithm with extended coverage */
	/* http://www.ese-metz.fr/~dedu/projects/bresenham/ */
	/* XXX - is this even correct ? the original doesn't look very good */
	int c, step_x, step_y, error;//, last_error;
	int end[2];
	int x, y, dx, dy, ddx, ddy;

	/* for 3d collisions */
	float h, step_h, dist, tmp[3];

	end[X] = (int)end_pos[X];
	end[Y] = (int)end_pos[Z];
	
	x = (int)start_pos[X];
	y = (int)start_pos[Z];

	math_sub_vec3(tmp, end_pos, start_pos);
	dist = math_norm_vec3(tmp);
	h = start_pos[Y];
	step_h = tmp[Y] / dist;
	
	dx = end[X] - x;
	dy = end[Y] - y;
	
	/* check first point */
	if(x < 0 || x >= game->map->width ||
	   y < 0 || y >= game->map->height)
		return 0;

	if(h > -1.0 && map_collision_square(game, x, y))
	{
		/* collision at x, y */
		ret[X] = x;
		ret[Y] = y;
		return 1;
	}

	if(dy < 0)
	{
		step_y = -1;
		dy = -dy;
	} else
		step_y = 1;

	if(dx < 0)
	{
		step_x = -1;
		dx = -dx;
	} else
		step_x = 1;

	ddx = 2 * dx;
	ddy = 2 * dy;
	
	if(ddx >= ddy)
	{
//		last_error = dx;
		error = dx;
		
		for(c = 0; c < dx; c++)
		{
			x += step_x;
			error += ddy;

			h += step_h;
			if(h < -1.0 && step_h < 0.0)
				/* gone over the map, no collision possible */
				return 0;
			
			if(error > ddx)
			{
				y += step_y;
				error -= ddx;
#if 0
				if(error + last_error < ddx)
				{
					if(h > -1.0 &&
					   map_collision_square(game, x, y - step_y))
					{
						/* collision at x, y - step_y */
						ret[X] = x;
						ret[Y] = y - step_y;
						return 1;
					}
				} else {
					if(error + last_error > ddx)
					{
						if(h > -1.0 &&
						   map_collision_square(game, x - step_x, y))
						{
							/* collision at x - step_x, y */
							ret[X] = x - step_x;
							ret[Y] = y;
							return 1;
						}
					} else {
						if(h > -1.0 &&
						   map_collision_square(game, x, y - step_y))
						{
							ret[X] = x;
							ret[Y] = y - step_y;
							return 1;
						}
						
						if(h > -1.0 &&
						   map_collision_square(game, x - step_x, y))
						{
							ret[X] = x - step_x;
							ret[Y] = y;
							return 1;
						}
					}
				}
#endif
			}

			if(x < 0 || x >= game->map->width ||
			   y < 0 || y >= game->map->height)
				return 0;

			if(h > -1.0 && map_collision_square(game, x, y))
			{
				ret[X] = x;
				ret[Y] = y;
				return 1;
			}
		}
	} else {
		//	last_error = dy;
		error = dy;			
		
		for(c = 0; c < dy; c++)
		{
			y += step_y;
			error += ddx;

			h += step_h;
			if(h < -1.0 && step_h < 0.0)
				/* gone over the map, no collision possible */
				return 0;
			
			if(error > ddy)
			{
				x += step_x;
				error = -ddy;
#if 0				
				if(error + last_error < ddy)
				{
					if(h > -1.0 &&
					   map_collision_square(game, x - step_x, y))
					{
						ret[X] = x - step_x;
						ret[Y] = y;
						return 1;
					}
				} else {
					if(error + last_error > ddy)
					{
						if(h > -1.0 &&
						   map_collision_square(game, x, y - step_y))
						{
							ret[X] = x;
							ret[Y] = y - step_y;
							return 1;
						}
					} else {
						if(h > -1.0 &&
						   map_collision_square(game, x - step_x, y))
						{
							ret[X] = x - step_x;
							ret[Y] = y;
							return 1;
						}
						
						if(h > -1.0 &&
						   map_collision_square(game, x, y - step_y))
						{
							ret[X] = x;
							ret[Y] = y - step_y;
							return 1;
						}
					}
				}
#endif
			}

			if(x < 0 || x >= game->map->width ||
			   y < 0 || y >= game->map->height)
				return 0;
			
			if(h > -1.0 &&
			   map_collision_square(game, x, y))
			{
				ret[X] = x;
				ret[Y] = y;
				return 1;
			}
		}
	}

	return 0;
}

int map_inside(struct game *game, int x, int y)
{
	if(x < 0 || x > game->map->width ||
	   y < 0 || y > game->map->height)
		return 0;
	
	return 1;
}

