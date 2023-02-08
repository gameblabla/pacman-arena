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

/* $Id: map.h,v 1.2 2003/11/22 17:32:10 nsubtil Exp $ */

#ifndef _MAP_H
#define _MAP_H

#define MAP_WALL_NONE 0
#define MAP_WALL_VERTICAL 1
#define MAP_WALL_HORIZONTAL 2
#define MAP_WALL_LL 3
#define MAP_WALL_LR 4
#define MAP_WALL_UL 5
#define MAP_WALL_UR 6
/* only ghosts may traverse this wall */
#define MAP_WALL_GHOST_ONLY 7

#define MAP_CONTENT_NONE 0
#define MAP_CONTENT_FOOD 1
#define MAP_CONTENT_PILL 2
#define MAP_CONTENT_TELEPORT 3

#define MAP_FLAGS_NONE 0
#define MAP_FLAG_PACMAN_START_POSITION 1
#define MAP_FLAG_GHOST_START_POSITION 2
#define MAP_FLAG_BOMB 4

/* pacman food */

/* eat once */
#define FOOD_TYPE_NORMAL 0
/* respawns after being eaten */
#define FOOD_TYPE_RESPAWN 1
/* respawns, disappears */
#define FOOD_TYPE_INTERMITTENT 2

#define FOOD_STATUS_ACTIVE 0
#define FOOD_STATUS_EATEN 1

struct pacman_food
{
	int type;
	int status;
	float time;

	/* INTERMITTENT: food disappears after being active for active_time seconds */
	float active_time;
	/* INTERMITTENT: food fades out during last fade_out seconds */
	float fade_out;
	
	/* INTERMITTENT/RESPAWN: food respawns after inactive_time seconds */
	float inactive_time;
	/* INTERMITTENT/RESPAWN: food fades in during last fade_in seconds */
	float fade_in;
};


/* pacman pills */

#define PILL_STATUS_ACTIVE 0
#define PILL_STATUS_EATEN 1

/* angular frequency (rad/s) */
#define PILL_BOB_FREQUENCY 4.0

struct pacman_pill
{
	int status;
	float phase;
};


/* teleports */

struct teleport
{
	int dest_x, dest_y;
	int direction;
};


/* map tile */

struct map_tile
{
	/* wall type */
	int wall;
	
	/* content type (food, pill, teleport, etc) */
	int content;
	/* content data */
	union 
	{
		struct pacman_food food;
		struct pacman_pill pill;
		struct teleport teleport;
	} c_data;

	/* tile flags */
	int flags;

	/* ghost direction towards nearest spawn on this tile */
	int ghost_dir;
	/* ghost direction when ghost is alive (for exits, etc) */
	int ghost_dir_alive;
};

struct map 
{
	int width;
	int height;
	
	struct map_tile **tiles;
};

#define MAP(map, x, y) map->tiles[y][x]
#define MAP_CONTENT_DATA(map, x, y) map->tiles[y][x].content_data
#define MAP_CAN_ENTER(map, x, y) (x >= 0 && x < map->width && y >= 0 && y < map->height && MAP(map, x, y).wall == MAP_WALL_NONE)// && MAP(map, x, y).content != MAP_CONTENT_TELEPORT)
#define MAP_CAN_ENTER_GHOST(map, x, y) (x >= 0 && x < map->width && y >= 0 && y < map->height && (MAP(map, x, y).wall == MAP_WALL_NONE || MAP(map, x, y).wall == MAP_WALL_GHOST_ONLY))// && MAP(map, x, y).content != MAP_CONTENT_TELEPORT)

#define MAP_FOOD(map, x, y) MAP(map, x, y).c_data.food
#define MAP_PILL(map, x, y) MAP(map, x, y).c_data.pill
#define MAP_TELEPORT(map, x, y) MAP(map, x, y).c_data.teleport

struct map *map_load_from_ascii(char **game_map, char **ghost_map, int width, int height);
void map_free(struct map *map);
void map_update(struct map *map, float delta);
int map_collision_square(struct game *game, int x, int y);
int map_detect_collision(struct game *game, float start_pos[3], float end_pos[3], int ret[2]);
int map_inside(struct game *game, int x, int y);

#endif
