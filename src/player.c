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
  "$Id: player.c,v 1.27 2003/11/30 17:43:55 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "object.h"
#include "audio.h"
#include "m_math.h"
#include "gfx.h"
#include "game.h"
#include "map.h"
#include "screen.h"
#include "render.h"
#include "shot.h"
#include "bomb.h"
#include "input.h"

#include "player.h"
#include "ghost.h"

GLfloat pacman_colors[2][4] = {
	{PACMAN_YELLOW}, {PACMAN_BLUE}
};
int num_pacman_colors = 2;

void player_reset(struct game *game)
{
	if(game->players)
		free(game->players);

	game->players = NULL;
	game->n_players = 0;
}

void player_add_new(struct game *game)
{
	struct player *new;
	
	game->players = realloc(game->players, sizeof(struct player) * (game->n_players + 1));
	new = &game->players[game->n_players];
	game->n_players++;
	
	new->state = PLAYER_STATE_STOPPED;
	new->pill_time = 0;

	new->position[X] = 0.0;
	new->position[Y] = -0.5;
	new->position[Z] = 0.0;
	
	new->direction = DIRECTION_UP;

	/* XXX - tudo isto não é suposto */
	new->model_moving = object_read_file("gfx/pacman-moving.3d", &new->frames_moving);
	new->model_stopped = object_read_file("gfx/pacman-stopped.3d", &new->frames_stopped);
	new->model_dying = object_read_file("gfx/pacman-dying.3d", &new->frames_dying);
	new->model_won = object_read_file("gfx/pacman-jumping.3d", &new->frames_won);

	new->current_frame = 0.0;

	/* XXX - ugly ? UGLY! */
	if(game->n_players == 1)
	{
		new->keys[KEY_UP] = SDLK_UP;
		new->keys[KEY_DOWN] = SDLK_DOWN;
		new->keys[KEY_LEFT] = SDLK_LEFT;
		new->keys[KEY_RIGHT] = SDLK_RIGHT;
		new->keys[KEY_FIRE] = SDLK_SPACE;
		new->keys[KEY_BOMB] = SDLK_b;
	} else {
		new->keys[KEY_UP] = SDLK_w;
		new->keys[KEY_DOWN] = SDLK_s;
		new->keys[KEY_LEFT] = SDLK_a;
		new->keys[KEY_RIGHT] = SDLK_d;
		new->keys[KEY_FIRE] = SDLK_f;
		new->keys[KEY_BOMB] = SDLK_g;
	}

	new->speed = 4.0;
	new->lives = PLAYER_START_LIVES;

	new->color = pacman_colors[(game->n_players - 1) % num_pacman_colors];
	new->camera = (struct camera *)malloc(sizeof(struct camera));
	new->camera->type = CAMERA_TYPE_LOOSE_TRAIL;
}

void player_update(struct game *game, int player_no, float delta)
{
	float new_position[3], vec[2];
	float dest_x, dest_z, frac_x, frac_z;
	int nx, ny;
	int keep_moving = 0, verify = 0, update_map = 0;
	struct player *p = &game->players[player_no];
	struct map *map = game->map;

	if(p->state == PLAYER_STATE_DEAD)
	{
		if(p->current_frame >= (float)(p->frames_dying - 1))
		{
			p->lives--;

			if(p->lives != 0)
			{
				/* colocar no jogo */
				p->state = PLAYER_STATE_STOPPED;
				p->position[X] = p->start_position[X];
				p->position[Y] = p->start_position[Y];
				p->position[Z] = p->start_position[Z];
				p->direction = DIRECTION_UP;
				p->pill_time = 0.0;
				p->speed = 4.0;
				p->current_frame = 0.0;
			}

			return;
		}

		/* actualizar frames */
		p->current_frame += delta * ANIM_FPS;
		return;
	}

	new_position[X] = p->position[X];
	new_position[Y] = p->position[Y];
	new_position[Z] = p->position[Z];
	
	if(p->state == PLAYER_STATE_MOVING)
	{
		frac_x = p->position[X] - (float)((int)p->position[X]);
		frac_z = p->position[Z] - (float)((int)p->position[Z]);
		
		dest_x = (int)p->position[X] + 0.5;
		dest_z = (int)p->position[Z] + 0.5;
		
		switch(p->direction)
		{
		case DIRECTION_UP:
			new_position[Z] += MIN(delta * p->speed, 0.5);

			if(frac_z > 0.5)
				dest_z += 1.0;
			
			if(input_kstate(p->keys[KEY_UP]))
				keep_moving = 1;

			if(new_position[Z] > dest_z)
			{
				update_map = 1;

				if(keep_moving)
				{
					p->position[X] = new_position[X];
					p->position[Z] = new_position[Z];
				} else {
					p->position[X] = dest_x;
					p->position[Z] = dest_z;
					p->state = PLAYER_STATE_STOPPED;
				}
			} else {
				if(MAP_CAN_ENTER(map, (int)dest_x, (int)dest_z))
				{
					p->position[X] = new_position[X];
					p->position[Z] = new_position[Z];
				} else {
					p->position[X] = dest_x;
					p->position[Z] = dest_z - 1.0;
					p->state = PLAYER_STATE_STOPPED;
				}
			}
			
			break;
			
		case DIRECTION_DOWN:
			new_position[Z] -= MIN(delta * p->speed, 0.5);

			if(frac_z < 0.5)
				dest_z -= 1.0;

			if(p->camera->type == CAMERA_TYPE_TOMB_RAIDER)
			{
				if(input_kstate(p->keys[KEY_UP]))
					keep_moving = 1;
			} else {
				if(input_kstate(p->keys[KEY_DOWN]))
					keep_moving = 1;
			}
			
			if(new_position[Z] < dest_z)
			{
				update_map = 1;

				if(keep_moving)
				{
					p->position[X] = new_position[X];
					p->position[Z] = new_position[Z];
				} else {
					p->position[X] = dest_x;
					p->position[Z] = dest_z;
					p->state = PLAYER_STATE_STOPPED;
				}
			} else {
				if(MAP_CAN_ENTER(map, (int)dest_x, (int)dest_z))
				{
					p->position[X] = new_position[X];
					p->position[Z] = new_position[Z];
				} else {
					p->position[X] = dest_x;
					p->position[Z] = dest_z + 1.0;
					p->state = PLAYER_STATE_STOPPED;
				}
			}

			break;
			
		case DIRECTION_LEFT:
			new_position[X] -= MIN(delta * p->speed, 0.5);

			if(frac_x < 0.5)
				dest_x -= 1.0;

			if(p->camera->type == CAMERA_TYPE_TOMB_RAIDER)
			{
				if(input_kstate(p->keys[KEY_UP]))
					keep_moving = 1;
			} else {
				if(input_kstate(p->keys[KEY_LEFT]))
					keep_moving = 1;
			}
			
			if(new_position[X] < dest_x)
			{
				update_map = 1;

				if(keep_moving)
				{
					p->position[X] = new_position[X];
					p->position[Z] = new_position[Z];
				} else {
					p->position[X] = dest_x;
					p->position[Z] = dest_z;
					p->state = PLAYER_STATE_STOPPED;
				}
			} else {
				if(MAP_CAN_ENTER(map, (int)dest_x, (int)dest_z))
				{
					p->position[X] = new_position[X];
					p->position[Z] = new_position[Z];
				} else {
					p->position[X] = dest_x + 1.0;
					p->position[Z] = dest_z;
					p->state = PLAYER_STATE_STOPPED;
				}
			}

			break;
			
		case DIRECTION_RIGHT:
			new_position[X] += MIN(delta * p->speed, 0.5);

			if(frac_x > 0.5)
				dest_x += 1.0;

			if(p->camera->type == CAMERA_TYPE_TOMB_RAIDER)
			{
				if(input_kstate(p->keys[KEY_UP]))
					keep_moving = 1;
			} else {			
				if(input_kstate(p->keys[KEY_RIGHT]))
					keep_moving = 1;
			}
			
			if(new_position[X] > dest_x)
			{
				update_map = 1;

				if(keep_moving)
				{
					p->position[X] = new_position[X];
					p->position[Z] = new_position[Z];
				} else {
					p->position[X] = dest_x;
					p->position[Z] = dest_z;
					p->state = PLAYER_STATE_STOPPED;
				}
			} else {
				if(MAP_CAN_ENTER(map, (int)dest_x, (int)dest_z))
				{
					p->position[X] = new_position[X];
					p->position[Z] = new_position[Z];
				} else {
					p->position[X] = dest_x - 1.0;
					p->position[Z] = dest_z;
					p->state = PLAYER_STATE_STOPPED;
				}
				
			}
			
			break;
		}
	}

	/* mapa */
	vec[X] = p->position[X] - (float)((int)p->position[X]) + 0.5;
	vec[Y] = p->position[Z] - (float)((int)p->position[Z]) + 0.5;

	if(update_map || math_norm_vec2(vec) < 0.1)
	{
		switch(MAP(map, (int)p->position[X], (int)p->position[Z]).content)
		{
		case MAP_CONTENT_FOOD:
			if(MAP(map, (int)p->position[X], (int)p->position[Z]).c_data.food.status == FOOD_STATUS_ACTIVE)
			{
				MAP(map, (int)p->position[X], (int)p->position[Z]).c_data.food.status = FOOD_STATUS_EATEN;
				audio_play_sample("sfx/chomp.wav");
			}
			break;
			
		case MAP_CONTENT_PILL:
			if(MAP(map, (int)p->position[X], (int)p->position[Z]).c_data.pill.status == PILL_STATUS_ACTIVE)
			{
				MAP(map, (int)p->position[X], (int)p->position[Z]).c_data.pill.status = PILL_STATUS_EATEN;
				audio_play_sample("sfx/glomp.wav");
				p->pill_time = 10.0;
				ghost_taint_all(game);
			}

			break;

		case MAP_CONTENT_TELEPORT:
			game_teleport_player(game, player_no);
			break;

/*
		case '0':
			if(p->direction == DIRECTION_LEFT)
				game_teleport_player(p, MAP_POS(game_map, (int)p->position[X],
								(int)p->position[Z]));
			break;

		case '1':
			if(p->direction == DIRECTION_RIGHT)
				game_teleport_player(p, MAP_POS(game_map, (int)p->position[X],
								(int)p->position[Z]));

			break;
*/
		}
	}

	/* teclas */
	if(p->camera->type == CAMERA_TYPE_TOMB_RAIDER)
	{
		/* change direction */
		switch(p->direction)
		{
		case DIRECTION_UP:
			/* left turns pacman left */
			if(input_kstate(p->keys[KEY_LEFT]))
			{
				p->direction = DIRECTION_LEFT;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_LEFT]);
			}
			
			/* right turns pacman right */
			if(input_kstate(p->keys[KEY_RIGHT]))
			{
				p->direction = DIRECTION_RIGHT;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_RIGHT]);
			}
			
			/* down turns pacman down */
			if(input_kstate(p->keys[KEY_DOWN]))
			{
				p->direction = DIRECTION_DOWN;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_DOWN]);
			}
				
			break;
				
		case DIRECTION_DOWN:
			/* left turns pacman right */
			if(input_kstate(p->keys[KEY_LEFT]))
			{
				p->direction = DIRECTION_RIGHT;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_LEFT]);
			}
			
			/* right turns pacman left */
			if(input_kstate(p->keys[KEY_RIGHT]))
			{
				p->direction = DIRECTION_LEFT;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_RIGHT]);
			}
			
			/* down turns pacman up */
			if(input_kstate(p->keys[KEY_DOWN]))
			{
				p->direction = DIRECTION_UP;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_DOWN]);
			}
			
			break;

		case DIRECTION_LEFT:
			/* left turns pacman down */
			if(input_kstate(p->keys[KEY_LEFT]))
			{
				p->direction = DIRECTION_DOWN;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_LEFT]);
			}
			
			/* right turns pacman up */
			if(input_kstate(p->keys[KEY_RIGHT]))
			{
				p->direction = DIRECTION_UP;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_RIGHT]);
			}
			
			/* down turns pacman right */
			if(input_kstate(p->keys[KEY_DOWN]))
			{
				p->direction = DIRECTION_RIGHT;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_DOWN]);
			}
			
			break;
			
		case DIRECTION_RIGHT:
			/* left turns pacman up */
			if(input_kstate(p->keys[KEY_LEFT]))
			{
				p->direction = DIRECTION_UP;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_LEFT]);
			}
			
			/* right turns pacman down */
			if(input_kstate(p->keys[KEY_RIGHT]))
			{
				p->direction = DIRECTION_DOWN;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_RIGHT]);
			}
			
			/* down turns pacman left */
			if(input_kstate(p->keys[KEY_DOWN]))
			{
				p->direction = DIRECTION_LEFT;
				p->state = PLAYER_STATE_STOPPED;
				input_kclear(p->keys[KEY_DOWN]);
			}
			
			break;
		}

		/* move */
		switch(p->direction)
		{
		case DIRECTION_UP:
			/* up moves pacman up */
			if(input_kstate(p->keys[KEY_UP]))
			{
				p->direction = DIRECTION_UP;
				if(p->state == PLAYER_STATE_STOPPED)
				{
					p->state = PLAYER_STATE_MOVING;
					verify = 1;
				}	
			}

			break;
			
		case DIRECTION_DOWN:
			/* up moves pacman down */
			if(input_kstate(p->keys[KEY_UP]))
			{
				p->direction = DIRECTION_DOWN;
				if(p->state == PLAYER_STATE_STOPPED)
				{
					p->state = PLAYER_STATE_MOVING;
					verify = 1;
				}
			}

			break;
			
		case DIRECTION_LEFT:
			/* up moves pacman left */
			if(input_kstate(p->keys[KEY_UP]))
			{
				p->direction = DIRECTION_LEFT;
				if(p->state == PLAYER_STATE_STOPPED)
				{
					p->state = PLAYER_STATE_MOVING;
					verify = 1;
				}
			}

			break;
			
		case DIRECTION_RIGHT:
			/* up moves pacman right */
			if(input_kstate(p->keys[KEY_UP]))
			{
				p->direction = DIRECTION_RIGHT;
				if(p->state == PLAYER_STATE_STOPPED)
				{	
					p->state = PLAYER_STATE_MOVING;
					verify = 1;
				}
			}

			break;
		}
	} else {
		if(input_kstate(p->keys[KEY_UP]) &&
		   (p->state == PLAYER_STATE_STOPPED ||
		    p->direction == DIRECTION_DOWN))
		{
			p->direction = DIRECTION_UP;
			p->state = PLAYER_STATE_MOVING;
			verify = 1;
		} else
			if(input_kstate(p->keys[KEY_LEFT]) &&
			   (p->state == PLAYER_STATE_STOPPED ||
			    p->direction == DIRECTION_RIGHT))
			{
				p->direction = DIRECTION_LEFT;
				p->state = PLAYER_STATE_MOVING;
				verify = 1;
			} else
				if(input_kstate(p->keys[KEY_RIGHT]) &&
				   (p->state == PLAYER_STATE_STOPPED ||
				    p->direction == DIRECTION_LEFT))
				{
					p->direction = DIRECTION_RIGHT;
					p->state = PLAYER_STATE_MOVING;
					verify = 1;
				} else
					if(input_kstate(p->keys[KEY_DOWN]) &&
					   (p->state == PLAYER_STATE_STOPPED ||
					    p->direction == DIRECTION_UP))
					{
						p->direction = DIRECTION_DOWN;
						p->state = PLAYER_STATE_MOVING;
						verify = 1;
					}
	}
	
	if(input_kstate(p->keys[KEY_FIRE]))
	{
		shot_new(game, player_no, SHOT_TYPE_ROCKET, p->speed);
		input_kclear(p->keys[KEY_FIRE]);
	}

	if(input_kstate(p->keys[KEY_BOMB]))
	{
		if((MAP(game->map, (int)p->position[X], (int)p->position[Z]).flags &
		   MAP_FLAG_BOMB) == 0)
			bomb_new(game, player_no, 4.0);

		input_kclear(p->keys[KEY_BOMB]);
	}
	
	if(verify)
	{
		/* validar nova direcção */
		nx = (int)p->position[X];
		ny = (int)p->position[Z];
	
		switch(p->direction)
		{
		case DIRECTION_UP:
			ny++;
			break;
		
		case DIRECTION_DOWN:
			ny--;
			break;
		
		case DIRECTION_LEFT:
			nx--;
			break;
		
		case DIRECTION_RIGHT:
			nx++;
			break;
		}
	
		if(!MAP_CAN_ENTER(map, nx, ny))
			p->state = PLAYER_STATE_STOPPED;

		/* align pacman with tile center */
		p->position[X] = (float)((int)p->position[X]) + 0.5;
		p->position[Z] = (float)((int)p->position[Z]) + 0.5;
	}

	/* frame da animação */
	switch(p->state)
	{
	case PLAYER_STATE_MOVING:
		if(p->direction == DIRECTION_LEFT || p->direction == DIRECTION_RIGHT)
			p->current_frame =
				(p->position[X] - (float)((int)p->position[X])) *
				p->frames_moving;
		else
			p->current_frame = (p->position[Z] - (float)((int)p->position[Z])) *
				p->frames_moving;

		break;

	case PLAYER_STATE_STOPPED:
		p->current_frame += delta * ANIM_FPS;
		break;
	}

	/* pill time! */
	if(p->pill_time != 0.0)
	{
		p->pill_time -= delta;
		if(p->pill_time <= 0.0)
		{
			p->pill_time = 0.0;
			ghost_untaint_all(game);
		}
	}
}

void player_kill(struct game *game, int player_no)
{
	struct player *player;

	player = &game->players[player_no];
	player->state = PLAYER_STATE_DEAD;
	player->current_frame = 0.0;
	audio_play_sample("sfx/pacman-die.wav");
}
