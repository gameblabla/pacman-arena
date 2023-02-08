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

/* $Id: bomb.h,v 1.3 2003/11/27 22:11:57 nsubtil Exp $ */
#ifndef _BOMB_H
#define _BOMB_H

#define BOMB_STATE_NONE 0
#define BOMB_STATE_COUNTDOWN 1
#define BOMB_STATE_EXPLOSION 2


/* constants */
#define BOMB_FLAME_TIME 0.5
#define BOMB_FLAME_TIME_SQ (BOMB_FLAME_TIME * BOMB_FLAME_TIME)

#define BOMB_PARTICLE_LIFE 0.2
#define BOMB_PARTICLE_FADE 0.3
#define BOMB_PARTICLE_RATE 300.0
#define BOMB_PARTICLE_SIZE 1.0
#define BOMB_PARTICLE_SPREAD 0.5
#define BOMB_PARTICLE_SPEED 0.02
#define BOMB_PARTICLE_SPEED_SPREAD 0.005
#define BOMB_PARTICLE_GRAVITY 0.01

/*
  flames start out from the bomb position with a starting speed V0
  they suffer constant acceleration such that they stop in BOMB_FLAME_TIME seconds
  thus:
  V0 = 2 * radius / BOMB_FLAME_TIME
  a = -2 * radius / BOMB_FLAME_TIME^2
*/

struct bomb
{
	int id;
	
	int owner;
	
	float position[3];
	float time;
	float countdown;
	float radius;
	int state;
	
	struct particle_src *trail_left;
	struct particle_src *trail_right;
	struct particle_src *trail_up;
	struct particle_src *trail_down;
	struct particle_src *explosion;

	struct object *model;
	int n_frames;
};

void bomb_new(struct game *game, int player_no, float radius);
void bomb_explode(struct game *game, int bomb_no);
void bomb_update(struct game *game, int bomb_no, float delta);
void bomb_remove(struct game *game, int bomb_no);

#endif
