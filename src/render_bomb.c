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
  "$Id: render_bomb.c,v 1.2 2003/11/30 17:43:55 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL.h>
#include <GL/gl.h>
#include <stdio.h>

#include "object.h"
#include "game.h"
#include "screen.h"
#include "render.h"
#include "player.h"
#include "particle.h"
#include "render_particle.h"

#include "bomb.h"

void bomb_render_opaque(struct game *game, int bomb_no)
{
	struct bomb *bomb;
	
	bomb = &game->bombs[bomb_no];
	if(bomb->state == BOMB_STATE_EXPLOSION)
		return;
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	
	glTranslatef(bomb->position[X], bomb->position[Y], bomb->position[Z]);
	render_setup_model_direction(DIRECTION_UP);
	
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	
	render_dlist(&bomb->model[(int)bomb->time % bomb->n_frames],
		     game->players[bomb->owner].color);
}

void bomb_render_translucent(struct game *game, int player_no, int bomb_no)
{
	struct bomb *bomb;
	
	bomb = &game->bombs[bomb_no];
	
	if(bomb->state == BOMB_STATE_EXPLOSION)
	{
		particle_render_src(game, player_no, bomb->trail_left);
		particle_render_src(game, player_no, bomb->trail_right);
		particle_render_src(game, player_no, bomb->trail_up);
		particle_render_src(game, player_no, bomb->trail_down);
		particle_render_src(game, player_no, bomb->explosion);
	}
}
