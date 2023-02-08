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

/* $Id: shot.h,v 1.3 2003/11/27 22:11:57 nsubtil Exp $ */
#ifndef _SHOT_H
#define _SHOT_H

#define SHOT_TYPE_NONE 0
#define SHOT_TYPE_ROCKET 1

#define SHOT_STATE_NONE 0
#define SHOT_STATE_ACTIVE 1
#define SHOT_STATE_DEAD 2

struct shot
{
	int id;

	int owner;
	int type;
	int state;

	float time;
	float speed;
	float position[4];
	int direction;

	struct object *model;
	int frames;

	struct light *light_src;
	struct particle_src *particle_trail;
};

void shot_new(struct game *game, int owner, int shot_type, float speed);
void shot_kill(struct game *game, int shot_no, float delta);
void shot_remove(struct game *game, int shot_no);
void shot_update(struct game *game, int shot_no, float delta);

#endif
