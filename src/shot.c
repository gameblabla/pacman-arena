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
  "$Id: shot.c,v 1.15 2003/11/22 17:32:10 nsubtil Exp $";

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

void shot_new(struct game *game, int owner, int shot_type, float speed)
{
	float p_direction[3], p_speed[3], p_color[3] = {1.0, 1.0, 1.0};
	struct shot *new;

	game->shots = realloc(game->shots, sizeof(struct shot) * (game->n_shots + 1));
	new = &game->shots[game->n_shots];
	game->n_shots++;

	new->owner = owner;
	new->type = shot_type;
	new->time = 0.0;
	new->speed = speed;
	new->position[X] = game->players[owner].position[X];
	new->position[Y] = game->players[owner].position[Y];
	new->position[Z] = game->players[owner].position[Z];
	new->position[W] = 1.0;
	new->direction = game->players[owner].direction;
	new->state = SHOT_STATE_ACTIVE;
	
	switch(shot_type)
	{
	case SHOT_TYPE_ROCKET:
		new->model = object_read_file("gfx/rocket.3d", &new->frames);
		switch(new->direction)
		{
		case DIRECTION_UP:
			p_speed[X] = 0.0;
			p_speed[Y] = 0.0;
			p_speed[Z] = 1.0;
			
			p_direction[X] = 0.0;
			p_direction[Y] = 0.0;
			p_direction[Z] = -1.0;
			break;
			
		case DIRECTION_DOWN:
			p_speed[X] = 0.0;
			p_speed[Y] = 0.0;
			p_speed[Z] = -1.0;
			
			p_direction[X] = 0.0;
			p_direction[Y] = 0.0;
			p_direction[Z] = 1.0;
			break;
			
		case DIRECTION_LEFT:
			p_speed[X] = -1.0;
			p_speed[Y] = 0.0;
			p_speed[Z] = 0.0;
			
			p_direction[X] = 1.0;
			p_direction[Y] = 0.0;
			p_direction[Z] = 0.0;
			break;
			
		case DIRECTION_RIGHT:
			p_speed[X] = 1.0;
			p_speed[Y] = 0.0;
			p_speed[Z] = 0.0;
			
			p_direction[X] = -1.0;
			p_direction[Y] = 0.0;
			p_direction[Z] = 0.0;
			break;
		}

		math_len_vec3(p_speed, p_speed, new->speed);
		
		new->particle_trail = particle_new_src(0.5,           /* particle life time */
						       1.2,           /* particle fade time */
						       200.0,         /* particle gen. rate */
						       1.0,           /* particle size */
						       new->position, /* fountain position */
						       p_direction,   /* emission direction */
						       p_speed,       /* fountain speed */
						       0.2,           /* radial spread */
						       1.0,          /* particle speed */
						       0.005,	      /* particle speed spread */
						       -2.0,   /* particle gravity */
						       p_color);      /* particle color */

		audio_play_sample("sfx/rocket-launch.wav");

/*
		new->light_src = light_new();
		light_defaults(new->light_src);
		new->light_src->linked_position = new->position;

		MATH_SET_VEC4(new->light_src->ambient, 0.0, 0.0, 0.0, 1.0);
		MATH_SET_VEC4(new->light_src->diffuse, 1.0, 1.0, 1.0, 1.0);
		MATH_SET_VEC4(new->light_src->specular, 1.0, 1.0, 1.0, 1.0);
		MATH_SET_VEC3(new->light_src->attn, 15.0, 15.0, 15.0);
		new->light_src->spot_exponent = 64.0;
*/
		break;		
	default:
		new->particle_trail = NULL;
	}
}

void shot_kill(struct game *game, int shot_no, float delta)
{
	struct shot *shot;
	
	shot = &game->shots[shot_no];

	if(shot->particle_trail)
	{
		shot->state = SHOT_STATE_DEAD;
		MATH_COPY_VEC3(shot->particle_trail->position, shot->position);
		shot->particle_trail->particle_rate = 0.0;
		particle_src_explode(shot->particle_trail, 150 + (int)(shot->speed * shot->speed),
				     1.0 + shot->speed / 2.0);

/*
		shot->light_src = light_new();
		light_defaults(shot->light_src);

		MATH_COPY_VEC4(shot->light_src->position, shot->position);
		MATH_SET_VEC4(shot->light_src->ambient, 0.0, 0.0, 0.0, 1.0);
		MATH_SET_VEC4(shot->light_src->diffuse, 1.0, 0.1, 0.1, 1.0);
		MATH_SET_VEC4(shot->light_src->specular, 1.0, 0.1, 0.1, 1.0);
		shot->light_src->spot_exponent = 64.0;
		MATH_SET_VEC3(shot->light_src->attn, 0.0, 0.0, 10.0);
*/	
		audio_play_sample("sfx/explosion.wav");
	} else
		shot_remove(game, shot_no);
}

void shot_remove(struct game *game, int shot_no)
{
	particle_free_src(game->shots[shot_no].particle_trail);
/*	light_release(game->shots[shot_no].light_src);*/
	memmove(&game->shots[shot_no], &game->shots[shot_no + 1], (game->n_shots - shot_no) * sizeof(struct shot));
	game->n_shots--;
	game->shots = realloc(game->shots, game->n_shots * sizeof(struct shot));
}

void shot_update(struct game *game, int shot_no, float delta)
{
	struct shot *shot;
	float new_position[3];
	int old_tile[2], new_tile[2];
	int c;


	shot = &game->shots[shot_no];

	if(shot->state == SHOT_STATE_DEAD)
	{
		if(shot->particle_trail)
		{
			particle_src_update(game, shot->particle_trail, delta);
/*
			if(shot->light_src)
			{
				math_scale_vec3(shot->light_src->diffuse, 0.95, shot->light_src->diffuse);
				math_scale_vec3(shot->light_src->specular, 0.95, shot->light_src->specular);
			}
*/
			if(particle_src_all_dead(shot->particle_trail))
				shot_remove(game, shot_no);
		} else
			shot_remove(game, shot_no);
		
		return;
	}
	
	shot->time += delta;
	
	old_tile[X] = (int)shot->position[X];
	old_tile[Y] = (int)shot->position[Z];
	
	new_position[Y] = shot->position[Y];

	switch(shot->direction)
	{
	case DIRECTION_UP:
		new_position[X] = shot->position[X];
		new_position[Z] = shot->position[Z] + delta * shot->speed;
		break;
		
	case DIRECTION_DOWN:
		new_position[X] = shot->position[X];
		new_position[Z] = shot->position[Z] - delta * shot->speed;
		break;
		
	case DIRECTION_LEFT:
		new_position[Z] = shot->position[Z];
		new_position[X] = shot->position[X] - delta * shot->speed;
		break;
		
	case DIRECTION_RIGHT:
		new_position[Z] = shot->position[Z];
		new_position[X] = shot->position[X] + delta * shot->speed;
		break;

	default:
		printf("invalid shot direction %d\n", shot->direction);
		exit(1);
	}

	new_tile[X] = (int)new_position[X];
	new_tile[Y] = (int)new_position[Z];

	if(new_tile[X] < 0 || new_tile[X] >= game->map->width ||
		new_tile[Y] < 0 || new_tile[Y] >= game->map->height)
	{
		/* shot went out of map */
		MATH_COPY_VEC3(shot->position, new_position);
		shot_kill(game, shot_no, delta);
		return;
	}

	/* check for collisions with walls along the way from old_tile to new_tile */
	switch(shot->direction)
	{
	case DIRECTION_UP:
		for(c = old_tile[Y]; c <= new_tile[Y]; c++)
			if(MAP(game->map, old_tile[X], c).wall != MAP_WALL_NONE)
			{
				/* boom! (old_tile[X], c) */
				MATH_SET_VEC3(shot->position,
					      (float)old_tile[X] + 0.5,
					      shot->position[Y],
					      (float)c + 0.5 - 0.5);
				shot_kill(game, shot_no, delta);
				return;
			}
		break;
		
	case DIRECTION_DOWN:
		for(c = old_tile[Y]; c >= new_tile[Y]; c--)
			if(MAP(game->map, old_tile[X], c).wall != MAP_WALL_NONE)
			{
				/* boom! (old_tile[X], c) */
				MATH_SET_VEC3(shot->position,
					      (float)old_tile[X] + 0.5,
					      shot->position[Y],
					      (float)c + 0.5 + 0.5);
				shot_kill(game, shot_no, delta);
				return;
			}
		break;
		
	case DIRECTION_LEFT:
		for(c = old_tile[X]; c >= new_tile[X]; c--)
			if(MAP(game->map, c, old_tile[Y]).wall != MAP_WALL_NONE)
			{
				/* boom! (c, old_tile[Y]) */
				MATH_SET_VEC3(shot->position,
					      (float)c + 0.5 + 0.5,
					      shot->position[Y],
					      (float)old_tile[Y] + 0.5);
				shot_kill(game, shot_no, delta);
				return;
			}
		break;
		
	case DIRECTION_RIGHT:
		for(c = old_tile[X]; c <= new_tile[X]; c++)
			if(MAP(game->map, c, old_tile[Y]).wall != MAP_WALL_NONE)
			{
				/* boom! (c, old_tile[Y]) */
				MATH_SET_VEC3(shot->position,
					      (float)c + 0.5 - 0.5,
					      shot->position[Y],
					      (float)old_tile[Y] + 0.5);
				shot_kill(game, shot_no, delta);
				return;
			}
		break;
	}
	
	/* no collisions with walls, check collisions with ghosts */
	for(c = 0; c < game->n_ghosts; c++)
	{
		float vec[3];
		
		math_sub_vec3(vec, game->ghosts[c].position, new_position);
		if(math_norm_vec3(vec) < 0.7)
		{
			/* boom! hit ghost c */
			if(game->ghosts[c].state == GHOST_STATE_ACTIVE)
			{
				ghost_kill(game, &game->ghosts[c]);
				MATH_COPY_VEC3(shot->position, game->ghosts[c].position);
				shot_kill(game, shot_no, delta);
				return;
			}
		}
	}
	
	/* collisions with players */
	for(c = 0; c < game->n_players; c++)
	{
		float vec[3];
		
		if(c == shot->owner)
			/* do not damage self */
			continue;

		math_sub_vec3(vec, game->players[c].position, new_position);
		if(math_norm_vec3(vec) < 0.7)
		{
			/* hit player c */
			player_kill(game, c);
			MATH_COPY_VEC3(shot->position, game->players[c].position);			
			shot_kill(game, shot_no, delta);
			return;
		}
	}

	MATH_COPY_VEC3(shot->position, new_position);
	particle_src_update(game, shot->particle_trail, delta);

	if(shot->type == SHOT_TYPE_ROCKET)
	{
		float tmp[3];

		math_len_vec3(tmp, shot->particle_trail->src_speed, 1.0);
		math_scale_vec3(tmp, 10.0 * delta, tmp);
		math_add_vec3(shot->particle_trail->src_speed, shot->particle_trail->src_speed, tmp);
		
		shot->speed += 10.0 * delta;
		shot->particle_trail->particle_rate += 20.0 * delta;
	}
}
