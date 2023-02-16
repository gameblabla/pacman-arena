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
  "$Id: render_shot.c,v 1.3 2003/11/30 17:43:55 nsubtil Exp $";

#include "common.h" // Default headers

#include "game.h"
#include "player.h"
#include "ghost.h"
#include "map.h"
#include "render.h"
#include "m_math.h"
#include "object.h"
#include "particle.h"
#include "audio.h"

#include "shot.h"
#include "render_shot.h"
#include "render_particle.h"

void shot_render_opaque(struct game *game, int shot_no)
{
	struct shot *shot;

	shot = &game->shots[shot_no];
	if(shot->state == SHOT_STATE_DEAD)
		return;
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	
	glTranslatef(shot->position[X], shot->position[Y], shot->position[Z]);
	render_setup_model_direction(shot->direction);
	
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	render_dlist(&shot->model[(int)shot->time % shot->frames],
		     game->players[shot->owner].color);
}

void shot_render_translucent(struct game *game, int player_no, int shot_no)
{
	struct shot *shot;
	
	shot = &game->shots[shot_no];
	
	if(shot->type == SHOT_TYPE_ROCKET)
		particle_render_src(game, player_no, shot->particle_trail);
}
