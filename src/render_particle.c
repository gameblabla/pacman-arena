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
  "$Id: render_particle.c,v 1.2 2003/11/22 17:32:10 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>

#include "game.h"
#include "player.h"
#include "object.h"
#include "render.h"
#include "gfx.h"
#include "m_math.h"
#include "map.h"

#include "particle.h"
#include "render_particle.h"

void particle_render_src(struct game *game, int player_no, struct particle_src *src)
{
	struct camera *camera;
	int c;
	static struct image_rgba32 *texture = NULL;
	
	/* XXX - different textures! */
	if(texture == NULL)
	{
		texture = gfx_get("gfx/particle.tga");
		gfx_alpha_from_intensity("gfx/particle.tga");
		gfx_upload_texture("gfx/particle.tga");
	}
	
	camera = game->players[player_no].camera;
	
	/* setup plane geometry */
	render_setup_plane_geometry(camera, src->particle_size);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

#if 0
	render_draw_colored_plane(src->position[X], src->position[Y], src->position[Z],
				  src->color, 1.0);
#else	
	/* XXX - display lists! */
	for(c = 0; c < src->n_particles; c++)
	{
		struct particle *p;
		
		p = &src->particles[c];
		
		if(p->state == PARTICLE_STATE_DEAD)
			continue;

		if(p->time > src->particle_life)
		{
			float alpha;
			
			alpha = 1.0 - (p->time - src->particle_life) / src->particle_fade;
			render_draw_colored_plane(p->position[X], p->position[Y], p->position[Z],
						  p->color, alpha);
		} else
			render_draw_colored_plane(p->position[X], p->position[Y], p->position[Z],
						  p->color, 1.0);
	}
#endif
	glDepthMask(GL_TRUE);
}

