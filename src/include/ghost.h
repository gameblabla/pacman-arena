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

/* $Id: ghost.h,v 1.4 2003/11/27 22:11:57 nsubtil Exp $ */

#ifndef _GHOST_H
#define _GHOST_H

#define GHOST_STATE_ACTIVE 0
#define GHOST_STATE_DYING 1
#define GHOST_STATE_DEAD 2
#define GHOST_STATE_RETURNING 3

#define GHOST_SPEED 4.0

struct ghost
{
	int id;

	/* normal, comido, ... */
	int state;

	/* posição em world-space */
	float position[3];
	/* direcção do movimento */
	int direction;
	/* velocidade (tiles / segundo) */
	float speed;
	/* tempo que resta até mudar de direcção */
	float time;

	/* animação para o fantasma */
	struct object *model_active;
	int frames_active;

	struct object *model_dying;
	int frames_dying;

	struct object *model_dead;
	int frames_dead;

	struct object *model_returning;
	int frames_returning;

	/* frame actual */
	float current_frame;

	/* distância máxima (vertical / horizontal) a que o fantasma detecta um jogador */
	int view_distance;

	int tainted;

	/* cor */
	GLfloat *color;
};

#define GHOST_GREEN 18.0/256.0, 222.0/256.0, 18.0/256.0, 1.0
#define GHOST_RED 246.0/256.0, 67.0/256.0, 18.0/256.0, 1.0
#define GHOST_MAGENTA 224.0/256.0, 18.0/256.0, 243.0/256.0, 1.0
#define GHOST_ORANGE 252.0/256.0, 122.0/256.0, 0.0, 1.0

extern GLfloat ghost_colors[4][4];
extern int num_ghost_colors;

void ghost_taint_all(struct game *game);
void ghost_untaint_all(struct game *game);
void ghost_reset_all(struct game *game);
void ghost_kill(struct game *game, struct ghost *g);
void ghost_update(struct game *game, int ghost_no, float delta);
void ghost_detect_ghost_collisions(struct game *game, struct ghost *g);
int ghost_in_position(struct game *game, int x, int y);
void ghost_detect_player_collisions(struct game *game, struct ghost *g);
int ghost_check_visibility(struct game *game, struct ghost *g, int *r_dist);
void ghost_new_direction(struct game *game, struct ghost *g);

#endif
