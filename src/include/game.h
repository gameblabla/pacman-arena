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

/* $Id: game.h,v 1.2 2003/11/22 17:32:10 nsubtil Exp $ */

#ifndef _GAME_H
#define _GAME_H

/* demo mode */
#define GAME_TYPE_DEMO 0
/* classic game */
#define GAME_TYPE_CLASSIC 1
/* classic w/food respawn */
#define GAME_TYPE_CLASSIC_FOODFIGHT 2

struct game
{
	int game_type;
	int current_level;

	int n_players;
	struct player *players;

	int n_ghosts;
	struct ghost *ghosts;

	int n_shots;
	struct shot *shots;

	int n_bombs;
	struct bomb *bombs;

	struct map *map;
	struct camera *demo_camera;

};

/* -- camera -- */
#define CAMERA_TYPE_LOOSE_TRAIL 0
#define CAMERA_TYPE_TOMB_RAIDER 1
#define CAMERA_TYPE_FIXED_CHASE 2
#define CAMERA_TYPE_LOOSE_CHASE 3
#define CAMERA_TYPE_DEMO 4
#define CAMERA_TYPE_FREE 5
#define CAMERA_NUM_TYPES 6

#define CAMERA_LOOSE_CHASE_HIGH_THRESHOLD 4.0
#define CAMERA_LOOSE_CHASE_LOW_THRESHOLD 3.5
#define CAMERA_LOOSE_CHASE_SPEED 4.0

#define CAMERA_LOOSE_TRAIL_THRESHOLD 4.0
#define CAMERA_LOOSE_TRAIL_SPEED 1.8 * 2
#define CAMERA_LOOSE_TRAIL_X_DIST_HIGH 2.0
#define CAMERA_LOOSE_TRAIL_X_DIST_LOW 0.8
#define CAMERA_LOOSE_TRAIL_Z_OFFSET -3.5
#define CAMERA_LOOSE_TRAIL_Z_DIST_HIGH 1.5
#define CAMERA_LOOSE_TRAIL_Z_DIST_LOW 0.2
#define CAMERA_LOOSE_TRAIL_Z_SPEED 2.0 * 2
#define CAMERA_LOOSE_TRAIL_Y_OFFSET -7.0
/* norma por seg. */
#define CAMERA_LOOSE_TRAIL_ROTATE_SPEED 2.5

#define CAMERA_DEMO_HEIGHT -10.0
#define CAMERA_DEMO_ROTATE_SPEED 20.0

#define CAMERA_TOMB_RAIDER_ROTATE_SPEED 3.0
#define CAMERA_TOMB_RAIDER_OFFSET 3.5
#define CAMERA_TOMB_RAIDER_Y_OFFSET -4.0
#define CAMERA_TOMB_RAIDER_SPEED 4.0

/* direcções de movimento */
#define DIRECTION_UP 0
#define DIRECTION_DOWN 1
#define DIRECTION_LEFT 2
#define DIRECTION_RIGHT 3
#define DIRECTION_COUNT 4

#define DIRECTION_STOPPED 5
#define DIRECTION_NONE 6

struct camera
{
	int type;
	
	float pos[3];
	float dir[3];
	float up[3];
};

/* frames / segundo */
#define ANIM_FPS 30.0

struct game *game_new(int game_type);
void game_free(struct game *game);
void game_run(struct game *game);
void game_draw_player_lives(struct game *game, int player_no, float delta);
void game_do_victory_loop(struct game *game);
void game_do_defeat_loop(struct game *game);
void game_reset(struct game *game);
int game_count_dots(struct game *game);
void game_teleport_player(struct game *game, int player);
void game_start(struct game *game);
void game_reset_camera(struct game *game, int player_no);
void game_update_camera(struct game *game, int player_no, float delta);
void game_update(struct game *game, float delta);
void game_draw_opaque_objects(struct game *game, int player_no);
void game_draw_translucent_objects(struct game *game, int player_no);

#endif
