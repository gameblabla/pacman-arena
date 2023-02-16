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
  "$Id: bomb.c,v 1.6 2003/11/22 17:32:09 nsubtil Exp $";

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

#include "bomb.h"

void bomb_new(struct game *game, int player_no, float radius)
{
	struct bomb *new;
	struct player *p;
	
	game->bombs = realloc(game->bombs, sizeof(struct bomb) * (game->n_bombs + 1));
	new = &game->bombs[game->n_bombs];
	game->n_bombs++;
	
	new->owner = player_no;
	p = &game->players[player_no];

	MAP(game->map, (int)p->position[X], (int)p->position[Z]).flags |= MAP_FLAG_BOMB;	
	MATH_COPY_VEC3(new->position, p->position);

	new->time = 0.0;
	new->countdown = 1.0;
	new->state = BOMB_STATE_COUNTDOWN;
	new->radius = radius;

	new->trail_left = NULL;
	new->trail_right = NULL;
	new->trail_up = NULL;
	new->trail_down = NULL;
	new->explosion = NULL;
	
	new->model = object_read_file("gfx/bomb.3d", &new->n_frames);
}

void bomb_explode(struct game *game, int bomb_no)
{
	struct bomb *b;
	float emission[3], speed[3];
	float color[3] = { 1.0, 0.8, 0.7 };
	
	b = &game->bombs[bomb_no];
	
        /*	
	  V0 = 2 * radius / BOMB_FLAME_TIME
	  a = -2 * radius / BOMB_FLAME_TIME^2
	*/

	MATH_SET_VEC3(emission, -1.0, 0.0, 0.0);
	MATH_SET_VEC3(speed, -2.0 * b->radius / BOMB_FLAME_TIME, 0.0, 0.0);
	b->trail_left = particle_new_src(BOMB_PARTICLE_LIFE,
					 BOMB_PARTICLE_FADE,
					 BOMB_PARTICLE_RATE,
					 BOMB_PARTICLE_SIZE,
					 b->position,
					 emission,
					 speed,
					 BOMB_PARTICLE_SPREAD,
					 BOMB_PARTICLE_SPEED,
					 BOMB_PARTICLE_SPEED_SPREAD,
					 BOMB_PARTICLE_GRAVITY,
					 color);

	MATH_SET_VEC3(emission, 1.0, 0.0, 0.0);
	MATH_SET_VEC3(speed, 2.0 * b->radius / BOMB_FLAME_TIME, 0.0, 0.0);
	b->trail_right = particle_new_src(BOMB_PARTICLE_LIFE,
					 BOMB_PARTICLE_FADE,
					 BOMB_PARTICLE_RATE,
					 BOMB_PARTICLE_SIZE,
					 b->position,
					 emission,
					 speed,
					 BOMB_PARTICLE_SPREAD,
					 BOMB_PARTICLE_SPEED,
					 BOMB_PARTICLE_SPEED_SPREAD,
					 BOMB_PARTICLE_GRAVITY,
					 color);

	MATH_SET_VEC3(emission, 0.0, 0.0, 1.0);
	MATH_SET_VEC3(speed, 0.0, 0.0, 2.0 * b->radius / BOMB_FLAME_TIME);
	b->trail_up = particle_new_src(BOMB_PARTICLE_LIFE,
					 BOMB_PARTICLE_FADE,
					 BOMB_PARTICLE_RATE,
					 BOMB_PARTICLE_SIZE,
					 b->position,
					 emission,
					 speed,
					 BOMB_PARTICLE_SPREAD,
					 BOMB_PARTICLE_SPEED,
					 BOMB_PARTICLE_SPEED_SPREAD,
					 BOMB_PARTICLE_GRAVITY,
					 color);

	MATH_SET_VEC3(emission, 0.0, 0.0, -1.0);
	MATH_SET_VEC3(speed, 0.0, 0.0, -2.0 * b->radius / BOMB_FLAME_TIME);
	b->trail_down = particle_new_src(BOMB_PARTICLE_LIFE,
					 BOMB_PARTICLE_FADE,
					 BOMB_PARTICLE_RATE,
					 BOMB_PARTICLE_SIZE,
					 b->position,
					 emission,
					 speed,
					 BOMB_PARTICLE_SPREAD,
					 BOMB_PARTICLE_SPEED,
					 BOMB_PARTICLE_SPEED_SPREAD,
					 BOMB_PARTICLE_GRAVITY,
					 color);

	MATH_SET_VEC3(emission, 0.0, 0.0, 0.0);
	MATH_SET_VEC3(speed, 0.0, 0.0, 0.0);
	b->explosion = particle_new_src(0.5,
					1.2,
					0.0,
					1.0,
					b->position,
					emission,
					speed,
					0.5,
					0.02,
					0.005,
					0.05,
					color);
	particle_src_explode(b->explosion, 300, 10.0);

	b->state = BOMB_STATE_EXPLOSION;
	b->time = 0.0;
	
	audio_play_sample("sfx/rocket-launch.wav");
	audio_play_sample("sfx/explosion.wav");
}

void bomb_update(struct game *game, int bomb_no, float delta)
{
	struct bomb *b;
	int c;
	
	b = &game->bombs[bomb_no];
	
	switch(b->state)
	{
	case BOMB_STATE_COUNTDOWN:
		b->time += delta;
		if(b->time >= b->countdown)
		{
			float new_delta;
			
			new_delta = b->time - b->countdown;
			bomb_explode(game, bomb_no);
			bomb_update(game, bomb_no, new_delta);
			return;
		}

		break;
		
	case BOMB_STATE_EXPLOSION:
		/*	
		  V0 = 2 * radius / BOMB_FLAME_TIME
		  a = -2 * radius / BOMB_FLAME_TIME^2

		  V = V0 + a * t
		*/

		b->time += delta;
		
		particle_src_update(game, b->trail_left, delta);
		math_len_vec3(b->trail_left->src_speed,
			      b->trail_left->src_speed,
			      2.0 * b->radius / BOMB_FLAME_TIME +
			      (-2.0 * b->radius / BOMB_FLAME_TIME_SQ) * b->time);
		b->trail_left->speed = 0.0;
		b->trail_left->particle_rate -= BOMB_PARTICLE_RATE / BOMB_FLAME_TIME * delta;
		
		particle_src_update(game, b->trail_right, delta);
		math_len_vec3(b->trail_right->src_speed,
			      b->trail_right->src_speed,
			      2.0 * b->radius / BOMB_FLAME_TIME +
			      (-2.0 * b->radius / BOMB_FLAME_TIME_SQ) * b->time);
		b->trail_right->speed = 0.0;
		b->trail_right->particle_rate -= BOMB_PARTICLE_RATE / BOMB_FLAME_TIME * delta;

		particle_src_update(game, b->trail_up, delta);
		math_len_vec3(b->trail_up->src_speed,
			      b->trail_up->src_speed,
			      2.0 * b->radius / BOMB_FLAME_TIME +
			      (-2.0 * b->radius / BOMB_FLAME_TIME_SQ) * b->time);
		b->trail_up->speed = 0.0;
		b->trail_up->particle_rate -= BOMB_PARTICLE_RATE / BOMB_FLAME_TIME * delta;

		particle_src_update(game, b->trail_down, delta);
		math_len_vec3(b->trail_down->src_speed,
			      b->trail_down->src_speed,
			      2.0 * b->radius / BOMB_FLAME_TIME +
			      (-2.0 * b->radius / BOMB_FLAME_TIME_SQ) * b->time);
		b->trail_down->speed = 0.0;
		b->trail_down->particle_rate -= BOMB_PARTICLE_RATE / BOMB_FLAME_TIME * delta;

		particle_src_update(game, b->explosion, delta);
		
		if(particle_src_all_dead(b->trail_left) &&
		   particle_src_all_dead(b->trail_right) &&
		   particle_src_all_dead(b->trail_up) &&
		   particle_src_all_dead(b->trail_down) &&
		   particle_src_all_dead(b->explosion))
		{
			bomb_remove(game, bomb_no);
			return;
		}
		
		/* collisions with ghosts */
		for(c = 0; c < game->n_ghosts; c++)
		{
			float vec[3];
			
			math_sub_vec3(vec, game->ghosts[c].position, b->trail_left->position);
			if(math_norm_vec3(vec) < 0.7)
			{
				/* hit ghost c */
				if(game->ghosts[c].state == GHOST_STATE_ACTIVE)
				{
					ghost_kill(game, &game->ghosts[c]);
					MATH_COPY_VEC3(b->trail_left->position,
						       game->ghosts[c].position);
				}
			}
			
			math_sub_vec3(vec, game->ghosts[c].position, b->trail_right->position);
			if(math_norm_vec3(vec) < 0.7)
			{
				/* hit ghost c */
				if(game->ghosts[c].state == GHOST_STATE_ACTIVE)
				{
					ghost_kill(game, &game->ghosts[c]);
					MATH_COPY_VEC3(b->trail_right->position,
						       game->ghosts[c].position);
				}
			}

			math_sub_vec3(vec, game->ghosts[c].position, b->trail_up->position);
			if(math_norm_vec3(vec) < 0.7)
			{
				/* hit ghost c */
				if(game->ghosts[c].state == GHOST_STATE_ACTIVE)
				{
					ghost_kill(game, &game->ghosts[c]);
					MATH_COPY_VEC3(b->trail_up->position,
						       game->ghosts[c].position);
				}
			}

			math_sub_vec3(vec, game->ghosts[c].position, b->trail_down->position);
			if(math_norm_vec3(vec) < 0.7)
			{
				/* hit ghost c */
				if(game->ghosts[c].state == GHOST_STATE_ACTIVE)
				{
					ghost_kill(game, &game->ghosts[c]);
					MATH_COPY_VEC3(b->trail_down->position,
						       game->ghosts[c].position);
				}
			}
		}
		
		break;
	}
}

void bomb_remove(struct game *game, int bomb_no)
{
	struct bomb *b;
	
	b = &game->bombs[bomb_no];
	
	MAP(game->map, (int)b->position[X], (int)b->position[Z]).flags &= ~MAP_FLAG_BOMB;
	
	particle_free_src(b->trail_left);
	particle_free_src(b->trail_right);
	particle_free_src(b->trail_up);
	particle_free_src(b->trail_down);
	particle_free_src(b->explosion);

	memmove(&game->bombs[bomb_no], &game->bombs[bomb_no + 1],
		(game->n_bombs - bomb_no) * sizeof(struct bomb));
	game->n_bombs--;
	game->bombs = realloc(game->bombs, game->n_bombs * sizeof(struct bomb));

}
