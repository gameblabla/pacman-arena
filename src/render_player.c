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
  "$Id: render_player.c,v 1.2 2003/11/30 17:43:55 nsubtil Exp $";

#include "common.h" // Default headers

#include "object.h"
#include "game.h"
#include "screen.h"
#include "render.h"

#include "player.h"

void player_render(struct player *p)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

	glTranslatef(p->position[X], p->position[Y], p->position[Z]);

	switch(p->direction)
	{
	case DIRECTION_RIGHT:
		/* virado para ex */
		glRotatef(90.0, 0.0, 1.0, 0.0);
		break;

	case DIRECTION_LEFT:
		/* virado para -ex */
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		break;

	case DIRECTION_DOWN:
		/* virado para -ez */
		glRotatef(180.0, 0.0, 1.0, 0.0);
		break;
	}

	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	if(p->pill_time > 0.0)
		glEnable(GL_LIGHT1);
	
	switch(p->state)
	{
	case PLAYER_STATE_STOPPED:
		render_dlist(&p->model_stopped[(int)p->current_frame % p->frames_stopped], p->color);
		break;

	case PLAYER_STATE_MOVING:
		render_dlist(&p->model_moving[(int)p->current_frame % p->frames_moving], p->color);
		break;
		
	case PLAYER_STATE_DEAD:
		render_dlist(&p->model_dying[(int)p->current_frame % p->frames_dying], p->color);
		break;

	case PLAYER_STATE_WON:
		render_dlist(&p->model_won[(int)p->current_frame % p->frames_won], p->color);
		break;
	}

	glDisable(GL_LIGHT1);
}
