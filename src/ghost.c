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
  "$Id: ghost.c,v 1.21 2003/11/22 17:07:42 nsubtil Exp $";

#include "common.h" // Default headers

#include "m_math.h"
#include "object.h"
#include "game.h"
#include "map.h"
#include "ghost.h"
#include "player.h"
#include "render.h"
#include "audio.h"

GLfloat ghost_colors[4][4] = {
	{GHOST_GREEN}, {GHOST_RED}, {GHOST_MAGENTA}, {GHOST_ORANGE}
};
int num_ghost_colors = 4;

void ghost_taint_all(struct game *game)
{
	int c;
	
	for(c = 0; c < game->n_ghosts; c++)
		if(!game->ghosts[c].tainted && game->ghosts[c].state == GHOST_STATE_ACTIVE)
		{
			game->ghosts[c].tainted = 1;
			game->ghosts[c].speed /= 2.0;
		}
}

void ghost_untaint_all(struct game *game)
{
	int c;
	
	for(c = 0; c < game->n_ghosts; c++)
		if(game->ghosts[c].tainted)
		{
			game->ghosts[c].tainted = 0;
			game->ghosts[c].speed *= 2.0;
		}
}

void ghost_reset_all(struct game *game)
{
	int cx, cy;
	struct map *map;

	map = game->map;
	
	if(game->ghosts)
		free(game->ghosts);

	game->ghosts = NULL;
	game->n_ghosts = 0;

	for(cx = 0; cx < map->width; cx++)
		for(cy = 0; cy < map->height; cy++)
			if(MAP(map, cx, cy).flags & MAP_FLAG_GHOST_START_POSITION)
			{
				struct ghost *new;

				game->n_ghosts++;
				game->ghosts = realloc(game->ghosts,
						       sizeof(struct ghost) * game->n_ghosts);

				new = &game->ghosts[game->n_ghosts - 1];
				new->state = GHOST_STATE_ACTIVE;

				new->position[X] = (float)cx + 0.5;
				new->position[Y] = -0.5;
				new->position[Z] = (float)cy + 0.5;

				new->direction = DIRECTION_UP;
				new->speed = GHOST_SPEED;

				new->model_active =
					object_read_file("gfx/ghost-green-moving.3d",
									&new->frames_active);

				new->model_dying =
					object_read_file("gfx/ghost-green-dying.3d",
									&new->frames_dying);

				new->model_dead = &new->model_dying[new->frames_dying - 1];
				new->frames_dead = 1;

				new->model_returning = 
					object_read_file("gfx/ghost-green-returning.3d",
									&new->frames_returning);

				new->current_frame = (float)(rand() % new->frames_active);
				new->time = 0.0;
				new->view_distance = 10;

				new->color = ghost_colors[(game->n_ghosts - 1) % num_ghost_colors];
				new->tainted = 0;
			}
}

void ghost_kill(struct game *game, struct ghost *g)
{
	g->state = GHOST_STATE_DYING;
	g->current_frame = 0.0;
	audio_play_sample("sfx/eat-ghost.wav");	
}

void ghost_update(struct game *game, int ghost_no, float delta)
{
	float new_position[3], vec[2];
	float frac_x, frac_z, dest_x, dest_z;
	struct ghost *g;
	struct map *map;
	int keep_moving = 1, verify = 0;
	
	g = &game->ghosts[ghost_no];
	map = game->map;
	
	switch(g->state)
	{
	case GHOST_STATE_DYING:
		g->current_frame += delta * ANIM_FPS;

		if(g->current_frame >= (float)g->frames_dying)
		{
			g->state = GHOST_STATE_DEAD;
			g->current_frame = 0.0;
			if(g->tainted)
			{
				g->tainted = 0;
				g->speed *= 2.0;
			}
			
		}

		return;

	case GHOST_STATE_RETURNING:
		g->current_frame += delta * ANIM_FPS;

		if(g->current_frame >= (float)g->frames_returning)
		{
			g->state = GHOST_STATE_ACTIVE;
			g->current_frame = 0.0;
		}

		return;
	}

	g->time -= delta;
	if(g->time <= 0.0)
		keep_moving = 0;
	
	new_position[X] = g->position[X];
	new_position[Y] = g->position[Y];
	new_position[Z] = g->position[Z];

	frac_x = g->position[X] - (float)((int)g->position[X]);
	frac_z = g->position[Z] - (float)((int)g->position[Z]);

	dest_x = (int)g->position[X] + 0.5;
	dest_z = (int)g->position[Z] + 0.5;
	
	switch(g->direction)
	{
	case DIRECTION_UP:
		new_position[Z] += MIN(delta * g->speed, 0.5);
		
		if(frac_z > 0.5)
			dest_z += 1.0;
		
		if(new_position[Z] > dest_z)
		{
			verify = 1;
			
			if(keep_moving)
			{
				g->position[X] = new_position[X];
				g->position[Z] = new_position[Z];
			} else {
				g->position[X] = dest_x;
				g->position[Z] = dest_z;
				ghost_new_direction(game, g);
			}
		} else {
			if(MAP_CAN_ENTER_GHOST(map, (int)dest_x, (int)dest_z))
			{
				g->position[X] = new_position[X];
				g->position[Z] = new_position[Z];
			} else {
				
				g->position[X] = dest_x;
				g->position[Z] = dest_z - 1.0;
				do {
					ghost_new_direction(game, g);
				} while(g->direction == DIRECTION_UP &&
					g->state != GHOST_STATE_DEAD);
			}
		}
		
		break;

	case DIRECTION_DOWN:
		new_position[Z] -= MIN(delta * g->speed, 0.5);
		
		if(frac_z < 0.5)
			dest_z -= 1.0;
		
		if(new_position[Z] < dest_z)
		{
			verify = 1;
			
			if(keep_moving)
			{
				g->position[X] = new_position[X];
				g->position[Z] = new_position[Z];
			} else {
				g->position[X] = dest_x;
				g->position[Z] = dest_z;
				ghost_new_direction(game, g);
			}
		} else {
			if(MAP_CAN_ENTER_GHOST(map, (int)dest_x, (int)dest_z))
			{
				g->position[X] = new_position[X];
				g->position[Z] = new_position[Z];
			} else {
				
				g->position[X] = dest_x;
				g->position[Z] = dest_z + 1.0;
				do  {
					ghost_new_direction(game, g);
				} while(g->direction == DIRECTION_DOWN &&
					g->state != GHOST_STATE_DEAD);
			}
		}
		
		break;
		
	case DIRECTION_LEFT:
		new_position[X] -= MIN(delta * g->speed, 0.5);
		
		if(frac_x < 0.5)
			dest_x -= 1.0;
		
		if(new_position[X] < dest_x)
		{
			verify = 1;
			
			if(keep_moving)
			{
				g->position[X] = new_position[X];
				g->position[Z] = new_position[Z];
			} else {
				g->position[X] = dest_x;
				g->position[Z] = dest_z;
				ghost_new_direction(game, g);
			}
		} else {
			if(MAP_CAN_ENTER_GHOST(map, (int)dest_x, (int)dest_z))
			{
				g->position[X] = new_position[X];
				g->position[Z] = new_position[Z];
			} else {
				
				g->position[X] = dest_x + 1.0;
				g->position[Z] = dest_z;
				do {
					ghost_new_direction(game, g);
				} while(g->direction == DIRECTION_LEFT &&
					g->state != GHOST_STATE_DEAD);
			}
		}
		
		break;

	case DIRECTION_RIGHT:
		new_position[X] += MIN(delta * g->speed, 0.5);
		
		if(frac_x > 0.5)
			dest_x += 1.0;
		
		if(new_position[X] > dest_x)
		{
			verify = 1;
			
			if(keep_moving)
			{
				g->position[X] = new_position[X];
				g->position[Z] = new_position[Z];
			} else {
				g->position[X] = dest_x;
				g->position[Z] = dest_z;
				ghost_new_direction(game, g);
			}
		} else {
			if(MAP_CAN_ENTER_GHOST(map, (int)dest_x, (int)dest_z))
			{
				g->position[X] = new_position[X];
				g->position[Z] = new_position[Z];
			} else {
				
				g->position[X] = dest_x - 1.0;
				g->position[Z] = dest_z;
				do {
					ghost_new_direction(game, g);
				} while(g->direction == DIRECTION_RIGHT &&
					g->state != GHOST_STATE_DEAD);
			}
		}
		
		break;
	}
	
	if(g->time <= 0.0)
		ghost_new_direction(game, g);

	vec[X] = g->position[X] - (float)((int)g->position[X]);
	vec[Y] = g->position[Z] - (float)((int)g->position[Z]);
	if(verify || math_norm_vec2(vec) <= 0.1)
	{
		if(MAP(map, (int)g->position[X], (int)g->position[Z]).flags & MAP_FLAG_GHOST_START_POSITION)
			if(g->state == GHOST_STATE_DEAD)
			{
				g->state = GHOST_STATE_RETURNING;
				g->current_frame = 0.0;
				audio_play_sample("sfx/ghost-return.wav");
				return;
			}	

		switch(MAP(map, (int)g->position[X], (int)g->position[Z]).ghost_dir_alive)
		{
		case DIRECTION_UP:
			if(g->state == GHOST_STATE_ACTIVE)
			{
				g->direction = DIRECTION_UP;
				g->time = 1.0 / g->speed;
			}

			break;

		default:
		{
			int dir, dist;
			
			dir = ghost_check_visibility(game, g, &dist);
			if(dir != -1)
			{
				/* jogador por perto */
				g->direction = dir;
				g->time = dist / g->speed;
			}
		}	
		}
	}
	
	/* animação */
	g->current_frame = g->current_frame + delta * ANIM_FPS;
	
	ghost_detect_player_collisions(game, g);
	ghost_detect_ghost_collisions(game, g);
}

void ghost_detect_ghost_collisions(struct game *game, struct ghost *g)
{
	int c;
	float vec[3];
	struct map *map;
	
	map = game->map;
	
	for(c = 0; c < game->n_ghosts; c++)
	{
		math_sub_vec3(vec, g->position, game->ghosts[c].position);

		if(math_norm_vec3(vec) < 0.7)
		{
			int gx, gy, ox, oy;

			if(math_norm_vec3(vec) == 0.0)
				/* sou eu */
				continue;

			if(g->direction == DIRECTION_STOPPED)
				/* já colidiu */
				continue;
			
			gx = (int)g->position[X];
			gy = (int)g->position[Z];

			ox = (int)game->ghosts[c].position[X];
			oy = (int)game->ghosts[c].position[Z];

			if(rand() % 2)
			{
				/* escapatória para g */
				if(MAP_CAN_ENTER_GHOST(map, gx, gy + 1) &&
				   !ghost_in_position(game, gx, gy + 1))
				{
					g->direction = DIRECTION_UP;
					g->time = 1.0;
				
//					ghosts[c].direction = DIRECTION_STOPPED;
//					ghosts[c].time = 0.25;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, gx, gy - 1) &&
				   !ghost_in_position(game, gx, gy - 1))
				{
					g->direction = DIRECTION_DOWN;
					g->time = 1.0;
				
//					ghosts[c].direction = DIRECTION_STOPPED;
//					ghosts[c].time = 0.25;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, gx - 1, gy) &&
				   !ghost_in_position(game, gx - 1, gy))
				{
					g->direction = DIRECTION_LEFT;
					g->time = 1.0;
				
//					ghosts[c].direction = DIRECTION_STOPPED;
//					ghosts[c].time = 0.25;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, gx + 1, gy) &&
				   !ghost_in_position(game, gx + 1, gy))
				{
					g->direction = DIRECTION_RIGHT;
					g->time = 1.0;
				
//					ghosts[c].direction = DIRECTION_STOPPED;
//					ghosts[c].time = 0.25;
					continue;
				}
			
				/* escapatória para ghosts[c] */
				g->direction = DIRECTION_STOPPED;
				g->time = 0.0;
			
				if(MAP_CAN_ENTER_GHOST(map, ox, oy + 1) &&
				   !ghost_in_position(game, ox, oy + 1))
				{
					game->ghosts[c].direction = DIRECTION_UP;
					game->ghosts[c].time = 1.0;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, ox, oy - 1) &&
				   !ghost_in_position(game, ox, oy - 1))
				{
					game->ghosts[c].direction = DIRECTION_DOWN;
					game->ghosts[c].time = 1.0;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, ox - 1, oy) &&
				   !ghost_in_position(game, ox - 1, oy))
				{
					game->ghosts[c].direction = DIRECTION_LEFT;
					game->ghosts[c].time = 1.0;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, ox + 1, oy) &&
				   !ghost_in_position(game, ox + 1, oy))
				{
					game->ghosts[c].direction = DIRECTION_RIGHT;
					game->ghosts[c].time = 1.0;
					continue;
				}
			
				game->ghosts[c].direction = DIRECTION_STOPPED;
				game->ghosts[c].time = 0.25;
			} else {
				/* escapatória para ghosts[c] */
				if(MAP_CAN_ENTER_GHOST(map, ox, oy + 1) &&
				   !ghost_in_position(game, ox, oy + 1))
				{
					game->ghosts[c].direction = DIRECTION_UP;
					game->ghosts[c].time = 1.0;

//					g->direction = DIRECTION_STOPPED;
//					g->time = 0.25;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, ox, oy - 1) &&
				   !ghost_in_position(game, ox, oy - 1))
				{
					game->ghosts[c].direction = DIRECTION_DOWN;
					game->ghosts[c].time = 1.0;

//					g->direction = DIRECTION_STOPPED;
//					g->time = 0.25;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, ox - 1, oy) &&
				   !ghost_in_position(game, ox - 1, oy))
				{
					game->ghosts[c].direction = DIRECTION_LEFT;
					game->ghosts[c].time = 1.0;

//					g->direction = DIRECTION_STOPPED;
//					g->time = 0.25;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, ox + 1, oy) &&
				   !ghost_in_position(game, ox + 1, oy))
				{
					game->ghosts[c].direction = DIRECTION_RIGHT;
					game->ghosts[c].time = 1.0;

//					g->direction = DIRECTION_STOPPED;
//					g->time = 0.25;
					continue;
				}
			
				game->ghosts[c].direction = DIRECTION_STOPPED;
				game->ghosts[c].time = 0.0;

				/* escapatória para g */
				if(MAP_CAN_ENTER_GHOST(map, gx, gy + 1) &&
				   !ghost_in_position(game, gx, gy + 1))
				{
					g->direction = DIRECTION_UP;
					g->time = 1.0;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, gx, gy - 1) &&
				   !ghost_in_position(game, gx, gy - 1))
				{
					g->direction = DIRECTION_DOWN;
					g->time = 1.0;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, gx - 1, gy) &&
				   !ghost_in_position(game, gx - 1, gy))
				{
					g->direction = DIRECTION_LEFT;
					g->time = 1.0;
					continue;
				}
			
				if(MAP_CAN_ENTER_GHOST(map, gx + 1, gy) &&
				   !ghost_in_position(game, gx + 1, gy))
				{
					g->direction = DIRECTION_RIGHT;
					g->time = 1.0;
					continue;
				}
			
				g->direction = DIRECTION_STOPPED;
				g->time = 0.25;
			}
		}
	}
}

int ghost_in_position(struct game *game, int x, int y)
{
	int c;
	
	for(c = 0; c < game->n_ghosts; c++)
		if((int)game->ghosts[c].position[X] == x &&
		   (int)game->ghosts[c].position[Z] == y)
			return 1;
	
	return 0;
}

void ghost_detect_player_collisions(struct game *game, struct ghost *g)
{
	int c;
	float vec[3];
	
	if(g->state != GHOST_STATE_ACTIVE)
		return;

	for(c = 0; c < game->n_players; c++)
	{
		if(game->players[c].state == PLAYER_STATE_DEAD)
			continue;
		
		math_sub_vec3(vec, g->position, game->players[c].position);
		if(math_norm_vec3(vec) < 0.7)
		{
			/* fantasma bateu num jogador */
			if(game->players[c].pill_time > 0.0)
			{
				/* uh oh, fantasma comido */
				ghost_kill(game, g);
				return;
			}

			switch(g->direction)
			{
			case DIRECTION_UP:
				g->direction = DIRECTION_DOWN;
				break;
				
			case DIRECTION_DOWN:
				g->direction = DIRECTION_UP;
				break;

			case DIRECTION_LEFT:
				g->direction = DIRECTION_RIGHT;
				break;
				
			case DIRECTION_RIGHT:
				g->direction = DIRECTION_LEFT;
				break;
			}
			
			g->time = 1.0 / g->speed;

			player_kill(game, c);
			return;
		}
	}
}

/*
  devolve a direcção de um jogador próximo (distância em dist)
  ou -1 se não houver
*/
int ghost_check_visibility(struct game *game, struct ghost *g, int *r_dist)
{
	int c, px, py, gx, gy, dist, player_no;
	struct map *map;
	
	map = game->map;
	gx = (int)g->position[X];
	gy = (int)g->position[Z];
	
	if(g->state == GHOST_STATE_DEAD)
		return -1;

	/* determinar visibilidade de jogadores próximos */
	for(player_no = 0; player_no < game->n_players; player_no++)
	{
		if(game->players[player_no].state == PLAYER_STATE_DEAD)
			continue;
		
		px = (int)game->players[player_no].position[X];
		py = (int)game->players[player_no].position[Z];

		if(py == gy)
		{
			dist = gx - px;
			if(dist < 0)
				dist = -dist;
			
			if(dist > g->view_distance)
				/* muito longe */
				continue;
			
			if(px < gx)
			{
				for(c = gx - 1; c > px; c--)
					if(!MAP_CAN_ENTER_GHOST(map, c, py))
						/* não chega lá */
						break;

				if(c == px)
				{
					/* jogador à esquerda */
					*r_dist = dist;

					if(game->players[player_no].pill_time > 0.0)
					{
						/* fugir! */
						*r_dist = 1;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X] + 1,
							(int)g->position[Z]))
							return DIRECTION_RIGHT;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X],
							(int)g->position[Z] + 1))
							return DIRECTION_UP;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X],
							(int)g->position[Z] - 1))
							return DIRECTION_DOWN;

						/* sem hipótese de fugir */
					}

					return DIRECTION_LEFT;
				}
			} else {
				for(c = gx + 1; c < px; c++)
					if(!MAP_CAN_ENTER_GHOST(map, c, py))
						break;
				
				if(c == px)
				{
					/* jogador à direita */
					*r_dist = dist;

					if(game->players[player_no].pill_time > 0.0)
					{
						/* fugir! */
						*r_dist = 1;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X] - 1,
							(int)g->position[Z]))
							return DIRECTION_LEFT;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X],
							(int)g->position[Z] + 1))
							return DIRECTION_UP;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X],
							(int)g->position[Z] - 1))
							return DIRECTION_DOWN;
					}

					return DIRECTION_RIGHT;
				}
			}
		}

		if(px == gx)
		{
			dist = gy - py;
			if(dist < 0)
				dist = -dist;
			
			if(dist > g->view_distance)
				continue;
			
			if(py < gy)
			{
				for(c = gy - 1; c > py; c--)
					if(!MAP_CAN_ENTER_GHOST(map, px, c))
						break;
				
				if(c == py)
				{
					/* jogador abaixo */
					*r_dist = dist;

					if(game->players[player_no].pill_time > 0.0)
					{
						*r_dist = 1;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X],
							(int)g->position[Z] + 1))
							return DIRECTION_UP;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X] - 1,
							(int)g->position[Z]))
							return DIRECTION_LEFT;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X] + 1,
							(int)g->position[Z]))
							return DIRECTION_RIGHT;
					}

					return DIRECTION_DOWN;
				}
			} else {
				for(c = gy + 1; c < py; c++)
					if(!MAP_CAN_ENTER_GHOST(map, px, c))
						break;
				
				if(c == py)
				{
					/* jogador acima */
					*r_dist = dist;

					if(game->players[player_no].pill_time > 0.0)
					{
						*r_dist = 1;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X],
							(int)g->position[Z] - 1))
							return DIRECTION_DOWN;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X] - 1,
							(int)g->position[Z]))
							return DIRECTION_LEFT;

						if(MAP_CAN_ENTER_GHOST(map,
							(int)g->position[X] + 1,
							(int)g->position[Z]))
							return DIRECTION_RIGHT;
					}

					return DIRECTION_UP;
				}
			}
		}
	}

	return -1;
}

void ghost_new_direction(struct game *game, struct ghost *g)
{
	int dir, dist;
	struct map *map;
	
	map = game->map;
	
	/* XXX - eek! */
	g->position[X] = (float)((int)g->position[X]) + 0.5;
	g->position[Z] = (float)((int)g->position[Z]) + 0.5;

	if(g->state == GHOST_STATE_DEAD)
	{
		g->time = 1.0 / g->speed;

		switch(MAP(map, (int)g->position[X], (int)g->position[Z]).ghost_dir)
		{
		case DIRECTION_UP:
			g->direction = DIRECTION_UP;
			break;

		case DIRECTION_DOWN:
			g->direction = DIRECTION_DOWN;
			break;

		case DIRECTION_LEFT:
			g->direction = DIRECTION_LEFT;
			break;

		case DIRECTION_RIGHT:
			g->direction = DIRECTION_RIGHT;
			break;

		default:
			if(MAP(map, (int)g->position[X], (int)g->position[Z]).flags & MAP_FLAG_GHOST_START_POSITION)
			{
				g->time = 0.0;
				break;
			}

			printf("ghost_new_direction: que raio está um (hmm...) a fazer no mapa ? (%d %d)\n", (int)g->position[X], (int)g->position[Z]);
			fflush(stdout);
			SDL_Quit();
			exit(1);

			break;
		}

		return;
	}

	dir = ghost_check_visibility(game, g, &dist);
	if(dir == -1)
	{
		/* não há jogadores visíveis, direcção aleatória */
		g->direction = rand() % DIRECTION_COUNT;
		g->time = (float)(rand() % 5) + 1.0;
	} else {
		g->direction = dir;
		g->time = dist / g->speed;
	}
}
