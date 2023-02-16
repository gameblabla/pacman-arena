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
  "$Id: render_ghost.c,v 1.2 2003/11/30 17:43:55 nsubtil Exp $";

#include "common.h" // Default headers

#include "object.h"
#include "game.h"
#include "ghost.h"
#include "screen.h"
#include "render.h"

#include "player.h"

void ghost_render(struct game *game, struct ghost *g)
{
	GLfloat blue[] = {0.1, 0.1, 1.0, 1.0};

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

	glTranslatef(g->position[X], g->position[Y], g->position[Z]);
	render_setup_model_direction(g->direction);

	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	switch(g->state)
	{
	case GHOST_STATE_ACTIVE:
		if(g->tainted && (game->players[0].pill_time > 3.0 ||
				  (int)(game->players[0].pill_time * 4.0) % 2 == 0))
			render_dlist(&g->model_active[(int)g->current_frame % g->frames_active],
				     blue);
		else
			render_dlist(&g->model_active[(int)g->current_frame % g->frames_active],
				     g->color);
		break;

	case GHOST_STATE_DYING:
		if(g->tainted && (game->players[0].pill_time > 3.0 ||
				  (int)(game->players[0].pill_time * 4.0) % 2 == 0))
			render_dlist(&g->model_dying[(int)g->current_frame % g->frames_dying],
				     blue);
		else
			render_dlist(&g->model_dying[(int)g->current_frame % g->frames_dying],
				     g->color);
		break;

	case GHOST_STATE_DEAD:
		render_dlist(&g->model_dead[(int)g->current_frame % g->frames_dead], g->color);
		break;

	case GHOST_STATE_RETURNING:
		render_dlist(&g->model_returning[(int)g->current_frame % g->frames_returning], g->color);
		break;
	}
#if 0
	glLoadIdentity();
	glTranslatef(g->position[X], g->position[Y], g->position[Z]);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	
	glVertex3f(0.0, 0.0, 0.0);
	switch(g->direction)
	{
	case DIRECTION_LEFT:
		glVertex3f(-g->time * g->speed, 0.0, 0.0);
		break;

	case DIRECTION_RIGHT:
		glVertex3f(g->time * g->speed, 0.0, 0.0);
		break;
		
	case DIRECTION_UP:
		glVertex3f(0.0, 0.0, g->time * g->speed);
		break;
		
	case DIRECTION_DOWN:
		glVertex3f(0.0, 0.0, -g->time * g->speed);
		break;
	}
	
	glEnd();
#endif
}
