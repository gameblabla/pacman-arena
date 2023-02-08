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
  "$Id: particle.c,v 1.13 2003/11/22 17:07:42 nsubtil Exp $";

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

struct particle_src *particle_new_src(float life, float fade, float rate, float size,
				      float position[3], float direction[3], float src_speed[3],
				      float spread, float speed, float speed_spread,
				      float gravity, float color[3])
{
	struct particle_src *new;
	
	new = (struct particle_src *)malloc(sizeof(struct particle_src));

	new->n_particles = 0;
	new->particles = NULL;

	new->particle_life = life;
	new->particle_fade = fade;
	new->particle_rate = rate;
	new->last_particle_time = 0.0;
	new->particle_size = size;
	new->position[X] = position[X];
	new->position[Y] = position[Y];
	new->position[Z] = position[Z];
	new->direction[X] = direction[X];
	new->direction[Y] = direction[Y];
	new->direction[Z] = direction[Z];
	new->src_speed[X] = src_speed[X];
	new->src_speed[Y] = src_speed[Y];
	new->src_speed[Z] = src_speed[Z];
	new->gravity = gravity;
	new->spread = spread;
	new->speed = speed;
	new->speed_spread = speed_spread;
	new->color[R] = color[R];
	new->color[G] = color[G];
	new->color[B] = color[B];
	
	new->light_src = NULL;

	return new;
}

int particle_add(struct particle_src *src)
{
	int c, ret = -1;
	struct particle *new;
	
	/* look for free particle */
	new = NULL;
	for(c = 0; c < src->n_particles; c++)
		if(src->particles[c].state == PARTICLE_STATE_DEAD)
		{
			new = &src->particles[c];
			ret = c;
		}
	
	if(new == NULL)
	{
		/* add a new particle */
		src->particles = realloc(src->particles,
					 (src->n_particles + 1) * sizeof(struct particle));
		new = &src->particles[src->n_particles];
		ret = src->n_particles;
		src->n_particles++;
	}
	
	new->state = PARTICLE_STATE_ACTIVE;
	new->time = 0.0;
	MATH_COPY_VEC3(new->position, src->position);
	MATH_COPY_VEC3(new->color, src->color);
	
	new->direction[X] = src->direction[X] + src->spread * (rand() - RAND_MAX/2) / RAND_MAX;
	new->direction[Y] = src->direction[Y] + src->spread * (rand() - RAND_MAX/2) / RAND_MAX;
	new->direction[Z] = src->direction[Z] + src->spread * (rand() - RAND_MAX/2) / RAND_MAX;

	new->speed = src->speed + src->speed_spread * (rand() - RAND_MAX/2) / RAND_MAX;
	math_len_vec3(new->direction, new->direction, new->speed);
	new->gravity = src->gravity;

	return ret;
}

void particle_src_update(struct game *game, struct particle_src *src, float delta)
{
	int c;
	
	for(c = 0; c < src->n_particles; c++)
		if(src->particles[c].state == PARTICLE_STATE_ACTIVE)
			particle_update(game, src, c, delta);

	if(src->particle_rate > 0.0)
	{
		float tmp[3];
		
		src->last_particle_time += delta;

		while(src->last_particle_time >= 1.0 / src->particle_rate)
		{
			int i;
			float old_position[3];
			int collision[2];
			
			MATH_COPY_VEC3(old_position, src->position);

			math_scale_vec3(tmp, 1.0 / src->particle_rate, src->src_speed);
			math_add_vec3(src->position, src->position, tmp);

			if(map_detect_collision(game, old_position, src->position, collision))
			{
				float vec[3], coll[3];
				float norm;
				
				math_sub_vec3(vec, old_position, src->position);
				math_len_vec3(vec, vec, 0.6);

				MATH_SET_VEC3(coll,
					      (float)collision[X] + 0.5,
					      -0.5,
					      (float)collision[Y] + 0.5);
				math_add_vec3(src->position, vec, coll);

				norm = math_norm_vec3(src->src_speed);
				MATH_SET_VEC3(src->src_speed,
					      0.0,
					      -1.0,
					      0.0);
				math_len_vec3(src->src_speed, src->src_speed, norm);

				norm = math_norm_vec3(src->direction);
				MATH_SET_VEC3(src->direction,
					      0.0,
					      1.0,
					      0.0);
				math_len_vec3(src->direction, src->direction, norm);
			}
			
			i = particle_add(src);
			src->last_particle_time -= 1.0 / src->particle_rate;
			particle_update(game, src, i, src->last_particle_time);
		}
	}
}

void particle_update(struct game *game, struct particle_src *src, int particle_no, float delta)
{
	struct particle *p;
	float tmp[3], old_position[3];
	int collision[2];

	p = &src->particles[particle_no];
	
	p->time += delta;
	if(p->time >= src->particle_life + src->particle_fade)
	{
		p->state = PARTICLE_STATE_DEAD;
		return;
	}

	MATH_COPY_VEC3(old_position, p->position);

	math_scale_vec3(tmp, delta, p->direction);
	math_add_vec3(p->position, p->position, tmp);
	if(p->position[Y] < 0.0 || !map_inside(game, (int)p->position[X], (int)p->position[Z]))
		p->direction[Y] += p->gravity * delta;

	if(!map_inside(game, (int)p->position[X], (int)p->position[Z]))
		return;

	if(p->position[Y] > 0.0 && p->direction[Y] > 0.0)
		/* hit the floor */
		p->direction[Y] = -0.5 * p->direction[Y];

	if(map_detect_collision(game, old_position, p->position, collision))
	{
		switch(MAP(game->map, collision[X], collision[Y]).wall)
		{
		case MAP_WALL_VERTICAL:
			if(old_position[X] < (float)collision[X] + 0.5)
				/* move left */
				p->position[X] = (float)(collision[X] - 1) + 0.9;
			else
				/* move right */
				p->position[X] = (float)collision[X] + 1.01;

			/* switch horizontal speed */
			p->direction[X] = -p->direction[X];
			
			/* reduce speed */
			math_len_vec3(p->direction, p->direction,
				      math_norm_vec3(p->direction) - 0.01);

			break;

		case MAP_WALL_HORIZONTAL:
			if(old_position[Z] < (float)collision[Y] + 0.5)
				/* move down */
				p->position[Z] = (float)(collision[Y] - 1) + 0.9;
			else
				/* move up */
				p->position[Z] = (float)collision[Y] + 1.01;
			
			/* switch vertical speed */
			p->direction[Z] = -p->direction[Z];

			/* reduce speed */

			math_len_vec3(p->direction, p->direction,
				      math_norm_vec3(p->direction) - 0.01);

			break;
		}
	}
}

void particle_free_src(struct particle_src *src)
{
	if(src->particles)
		free(src->particles);
	
	free(src);
}

int particle_src_all_dead(struct particle_src *src)
{
	int c;
	
	for(c = 0; c < src->n_particles; c++)
		if(src->particles[c].state == PARTICLE_STATE_ACTIVE)
			return 0;
	
	return 1;
}

void particle_src_explode(struct particle_src *src, int num_particles_explosion, float max_speed)
{
	float saved_position[3], saved_direction[3], saved_color[3], saved_speed, saved_gravity;
	int c;
	
	MATH_COPY_VEC3(saved_position, src->position);
	MATH_COPY_VEC3(saved_direction, src->direction);
	MATH_COPY_VEC3(saved_color, src->color);
	saved_speed = src->speed;
	saved_gravity = src->gravity;
	
	src->particle_size /= 2.0;
     	src->gravity = 5.0;

	for(c = 0; c < num_particles_explosion; c++)
	{
		src->color[R] = 0.9 + (rand() / (float)RAND_MAX) * 0.1;
		src->color[G] = 0.7 + (rand() / (float)RAND_MAX) * 0.2;
		src->color[B] = 0.5 + (rand() / (float)RAND_MAX) * 0.1;
		
		src->direction[X] = (rand() / (float)RAND_MAX) * 2.0 - 1.0;
		src->direction[Y] = (rand() / (float)RAND_MAX) * 2.0 - 2.0;
		src->direction[Z] = (rand() / (float)RAND_MAX) * 2.0 - 1.0;

		src->speed = (rand() / (float)RAND_MAX) * max_speed;
		
		particle_add(src);		
	}
	
	MATH_COPY_VEC3(src->direction, saved_direction);
	src->speed = saved_speed;
	src->gravity = saved_gravity;
	src->particle_size *= 2.0;
}

