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
  "$Id: game.c,v 1.71 2003/11/30 17:43:55 nsubtil Exp $";

#include "common.h" // Default headers

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
#include "particle.h"
#include "input.h"

#include "player.h"
#include "ghost.h"

#include "render_map.h"
#include "render_ghost.h"
#include "render_player.h"
#include "render_shot.h"
#include "render_bomb.h"

#include "level-0.h"
//#include "level-classic.h"
//#include "level-demo.h"
//#include "level-1.h"

#include "config.h"

struct game *game_new(int game_type)
{
	struct game *ret;
	
	ret = (struct game *)malloc(sizeof(struct game));
	ret->game_type = game_type;
	ret->current_level = -1;
	ret->n_players = 0;
	ret->players = NULL;
	ret->n_ghosts = 0;
	ret->ghosts = NULL;
	ret->n_shots = 0;
	ret->bombs = NULL;
	ret->n_bombs = 0;
	ret->shots = NULL;
	ret->map = NULL;
	ret->demo_camera = NULL;

	return ret;
}

void game_free(struct game *game)
{
	if(game->players)
		free(game->players);
	if(game->ghosts)
		free(game->ghosts);
	if(game->map)
		map_free(game->map);

	free(game);
}

void game_run(struct game *game)
{
	int last_update, ticks;
	int frames;
	int c;
	
	input_reset();

	last_update = ticks = SDL_GetTicks();

	game_start(game);

	for(c = 0; c < game->n_players; c++)
	{
		render_reset_camera(game->players[c].camera);
		game_reset_camera(game, c);
	}

	frames = 0;

	audio_play_music("sfx/tranzy.ogg");

	for(;;)
	{
		int diff;

		input_update();
		
		if(input_kstate(GAME_FORCE_EXIT_BUTTON))
		{
			audio_fade_music(500);
			SDL_Delay(500);
			return;
		}

		diff = SDL_GetTicks() - last_update;
		game_update(game, (float)diff / 1000);
		last_update = SDL_GetTicks();

		render_start_frame();
		
		for(c = 0; c < game->n_players; c++)
		{
			render_setup(game, c);
			
			map_render_opaque_objects(game);
			game_draw_opaque_objects(game, c);
			map_render_translucent_objects(game, c);
			game_draw_translucent_objects(game, c);
#ifndef LOW_END
			game_draw_player_lives(game, c, (float)diff / 1000);
#endif
		}

/*		
		if(game->n_players == 1)
			render_draw_logo();
*/

		render_finish_frame();
		frames++;

		if(SDL_GetTicks() - ticks >= 1000)
		{
			#ifdef LOGGING_DEBUG
			printf("%d FPS\n", frames);
			#endif
			ticks = SDL_GetTicks();
			frames = 0;
		}

		if(game_count_dots(game) == 0)
		{
			game_do_victory_loop(game);
			return;
		}

		if(game->players[0].lives == 0)
		{
			game_do_defeat_loop(game);
			return;
		}

		SDL_Delay(0);
	}
}

void game_draw_player_lives(struct game *game, int player_no, float delta)
{
	GLfloat light0_position[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat light0_direction[] = { 0.0, 0.5, 1.5 };
	GLfloat inactive_color[] = { 0.5, 0.5, 0.5, 1.0 };
	
	int c;
	struct player *p;
	static float frame = 0.0;

	struct viewport *vp;

	vp = screen_get_viewport(player_no);
	p = &game->players[player_no];
	frame += delta * ANIM_FPS;
	
	for(c = 0; c < p->lives; c++)
	{		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
/*
		glViewport(0 + 70 * vp->width / 800 * c, 0,
			   150 * vp->width / 800, 150 * vp->width / 800);
*/
		glViewport(0 + 70 * vp->width / 800 * c, vp->ll_y,
			   150 * vp->width / 800, 150 * vp->width / 800);

		glFrustum(-1.0, 1.0,
			  -1.0, 1.0, 1.5, 3.0);
		mygluLookAt(0.0, 0.0, 0.0,
			  0.0, 0.0, 1.0,
			  0.0, 1.0, 0.0);

		glShadeModel(GL_SMOOTH);
		glEnable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);
	
		glClear(GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0.0, 0.0, 2.1);
	
		glRotatef(180.0, 0.0, 1.0, 0.0);
		glRotatef(180.0, 0.0, 0.0, 1.0);
		glRotatef(-75.0, 0.0, 1.0, 0.0);
		
		if(c == p->lives - 1)
			switch(p->state)
			{
			case PLAYER_STATE_MOVING:
			case PLAYER_STATE_STOPPED:
				render_dlist(&p->model_moving[(int)frame % p->frames_moving], p->color);
				break;
		
			case PLAYER_STATE_DEAD:
				render_dlist(&p->model_dying[(int)p->current_frame % p->frames_dying], p->color);
				break;
			}
		else
			render_dlist(&p->model_stopped[0], inactive_color);
	}
}

#if 1
/* XXX - this is for player 0 only */
void game_do_victory_loop(struct game *game)
{
	int last_update;
	struct image_rgba32 *level_complete;
	int tx_level_complete;
	struct viewport *vp;

	audio_fade_music(500);
	SDL_Delay(500);

	level_complete = gfx_get("gfx/level-complete.tga");
	gfx_alpha_from_key("gfx/level-complete.tga", 0, 0, 0);
	tx_level_complete = gfx_upload_texture("gfx/level-complete.tga");
	
	audio_play_music("sfx/hiscore.ogg");

	game->players[0].camera->type = CAMERA_TYPE_DEMO;
	
	last_update = SDL_GetTicks();
	
	game->players[0].state = PLAYER_STATE_WON;
	game->players[0].current_frame = 0.0;
	vp = screen_get_viewport(0);

	for(;;)
	{
		SDL_Event ev;
		int diff, c;
		float delta;
		
		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT)
			{
				SDL_Quit();
				exit(0);
			}
			
			if((ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) ||
				(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_SPACE)
				|| (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == GAME_CONFIRM_BUTTON)
				)
			{
				audio_fade_music(500);
				SDL_Delay(500);
				return;
			}
		}
		
		diff = SDL_GetTicks() - last_update;
		last_update = SDL_GetTicks();
		delta = (float)diff / 1000.0;
		
		for(c = 0; c < game->n_players; c++)
			game->players[c].current_frame += delta * ANIM_FPS;
		
		for(c = 0; c < game->n_ghosts; c++)
			game->ghosts[c].current_frame += delta * ANIM_FPS;

		game_update_camera(game, 0, delta);
		render_point_camera(game->players[0].camera,
				    game->players[0].position[X],
				    game->players[0].position[Y],
				    game->players[0].position[Z]);
		
		render_start_frame();

		render_setup(game, 0);
		map_render_opaque_objects(game);
		game_draw_opaque_objects(game, 0);
		map_render_translucent_objects(game, 0);
		game_draw_translucent_objects(game, 0);

/*		render_draw_logo();*/

		render_setup_2d(vp);

		if(level_complete->width > vp->width)
			render_draw_scaled_image("gfx/level-complete.tga",
						 0, 0,
						 vp->width,
						 level_complete->height * vp->height / vp->width);
		else
			render_draw_scaled_image("gfx/level-complete.tga",
						 (vp->width - level_complete->width) / 2, 0,
						 (vp->width + level_complete->width) / 2,
						 level_complete->height);

		render_finish_frame();
	}
}

/* XXX - player 0 only */
void game_do_defeat_loop(struct game *game)
{
	int last_update;
	struct image_rgba32 *game_over;
	int tx_game_over;
	struct viewport *vp;

	audio_fade_music(500);
	SDL_Delay(500);

	game_over = gfx_get("gfx/game-over.tga");
	gfx_alpha_from_key("gfx/game-over.tga", 0, 0, 0);
	tx_game_over = gfx_upload_texture("gfx/game-over.tga");

	audio_play_music("sfx/gameover.ogg");

	game->players[0].camera->type = CAMERA_TYPE_DEMO;
	
	last_update = SDL_GetTicks();
	vp = screen_get_viewport(0);

	for(;;)
	{
		int diff, c;
		float delta;
		
		input_update();

		if(input_kstate(SDLK_ESCAPE) || input_kstate(GAME_CONFIRM_BUTTON))
		{
			input_kclear(SDLK_ESCAPE);
			input_kclear(GAME_CONFIRM_BUTTON);
			audio_fade_music(500);
			SDL_Delay(500);
			return;
		}
		
		diff = SDL_GetTicks() - last_update;
		last_update = SDL_GetTicks();
		delta = (float)diff / 1000.0;
/*		
		for(c = 0; c < n_players; c++)
			players[c].current_frame += delta * ANIM_FPS;
*/	
		for(c = 0; c < game->n_ghosts; c++)
			game->ghosts[c].current_frame += delta * ANIM_FPS;

		game_update_camera(game, 0, delta);
		render_point_camera(game->players[0].camera,
				    game->players[0].position[X],
				    game->players[0].position[Y],
				    game->players[0].position[Z]);
		
		render_start_frame();

		render_setup(game, 0);
		map_render_opaque_objects(game);
		game_draw_opaque_objects(game, 0);
		map_render_translucent_objects(game, 0);
		game_draw_translucent_objects(game, 0);

/*		render_draw_logo();*/

		render_setup_2d(vp);

		if(game_over->width > vp->width)
			render_draw_scaled_image("gfx/game-over.tga",
						 0, 0,
						 vp->width,
						 game_over->height * vp->height / vp->width);
		else
			render_draw_scaled_image("gfx/game-over.tga",
						 (vp->width - game_over->width) / 2, 0,
						 (vp->width + game_over->width) / 2,
						 game_over->height);
		render_finish_frame();
	}
}
#else
void game_do_victory_loop(struct game *game)
{
}

void game_do_defeat_loop(struct game *game)
{
}
#endif

void game_reset(struct game *game)
{
	player_reset(game);
	glDisable(GL_LIGHT1);
}

int game_count_dots(struct game *game)
{
	struct map *map;
	int x, y, ret;
	
	map = game->map;
	ret = 0;
	
	for(x = 0; x < map->width; x++)
		for(y = 0; y < map->height; y++)
			if(MAP(map, x, y).content == MAP_CONTENT_FOOD &&
			   MAP(map, x, y).c_data.food.status == FOOD_STATUS_ACTIVE)
				ret++;

	return ret;
}

void game_teleport_player(struct game *game, int player)
{
	struct player *p;
	struct map *map;
	int x, y;
	int dx, dy;
	
	p = &game->players[player];
	map = game->map;
	
	x = (int)p->position[X];
	y = (int)p->position[Z];
	
	if(MAP(map, x, y).content != MAP_CONTENT_TELEPORT)
		return;

	if(MAP_TELEPORT(map, x, y).direction != p->direction)
		return;
	
	dx = MAP_TELEPORT(map, x, y).dest_x;
	dy = MAP_TELEPORT(map, x, y).dest_y;

	p->position[X] = (float)dx + 0.5;
	p->position[Z] = (float)dy + 0.5;
}

void game_start(struct game *game)
{
	struct map *map;
	int c, cx, cy;
	
	/* distribuir os jogadores pelos spawn points */
	c = 0;
	map = game->map;
	
	for(cx = 0; cx < map->width; cx++)
		for(cy = 0; cy < map->height; cy++)
			if(c < game->n_players &&
			   MAP(map, cx, cy).flags & MAP_FLAG_PACMAN_START_POSITION)
			{
				/* spawn */
				game->players[c].position[X] = (float)cx + 0.5;
				game->players[c].position[Z] = (float)cy + 0.5;

				game->players[c].start_position[X] = (float)cx + 0.5;
				game->players[c].start_position[Y] = game->players[c].position[Y];
				game->players[c].start_position[Z] = (float)cy + 0.5;

				c++;
				
				if(c == game->n_players)
					break;


			}
	
	if(c != game->n_players)
	{
		printf("game_start: tanta gente (%d)!\n", game->n_players);
		SDL_Quit();
		exit(1);
	}

	ghost_reset_all(game);
}

void game_reset_camera(struct game *game, int player_no)
{
	struct camera *cam;
	struct player *player;

	if(player_no == -1)
	{
		player = NULL;
		cam = game->demo_camera;
	} else {
		player = &game->players[player_no];
		cam = player->camera;
	}
	
	switch(cam->type)
	{
	case CAMERA_TYPE_FIXED_CHASE:
		break;
		
	case CAMERA_TYPE_LOOSE_CHASE:
		render_reset_camera(cam);
		render_move_camera(cam,
				   player->position[X],
				   -3.5,
				   player->position[Z] - 3.5);
		break;

	case CAMERA_TYPE_LOOSE_TRAIL:
		render_reset_camera(cam);
		render_move_camera(cam,
				   0.0, CAMERA_LOOSE_TRAIL_Y_OFFSET, CAMERA_LOOSE_TRAIL_Z_OFFSET);
		render_point_camera(cam,
				    player->position[X],
				    player->position[Y],
				    player->position[Z]);

		break;

	case CAMERA_TYPE_DEMO:
		render_reset_camera(cam);
		render_move_camera(cam, game->map->width / 2 + 0.5, CAMERA_DEMO_HEIGHT, 0.0);
		render_point_camera(cam, game->map->width / 2 + 0.5, 0.0, game->map->height / 2 + 0.5);
		break;

	case CAMERA_TYPE_TOMB_RAIDER:
		render_reset_camera(cam);
		render_move_camera(cam,
				   0.0, CAMERA_TOMB_RAIDER_Y_OFFSET, -CAMERA_TOMB_RAIDER_OFFSET);
		render_point_camera(cam,
				    player->position[X],
				    player->position[Y],
				    player->position[Z]);
		
		break;

	case CAMERA_TYPE_FREE:
		break;
	}
}

void game_update_camera(struct game *game, int player_no, float delta)
{
	static int state = 0;
	struct camera *cam;
	struct player *player;
	
	if(player_no == -1)
	{
		/* magic value: demo camera */
		player = NULL;
		cam = game->demo_camera;
	} else {
		player = &game->players[player_no];
		cam = player->camera;
	}
	
	if(input_kstate(SDLK_c))
	{
		cam->type = (cam->type + 1) % CAMERA_NUM_TYPES;
		input_kclear(SDLK_c);
//		game_reset_camera(game, player_no);
	}
	
	switch(cam->type)
	{
		float v1[3], v2[3], vr[3], dist, d2;

	case CAMERA_TYPE_FIXED_CHASE:
		render_reset_camera(cam);
		render_move_camera(cam,
				   player->position[X],
				   -3.5,
				   player->position[Z] - 2.5);
		render_point_camera(cam,
				    player->position[X],
				    player->position[Y],
				    player->position[Z]);
		
		break;

	case CAMERA_TYPE_LOOSE_CHASE:
		render_point_camera(cam,
				    player->position[X],
				    player->position[Y],
				    player->position[Z]);

		v1[X] = player->position[X];
		v1[Y] = cam->pos[Y];
		v1[Z] = player->position[Z];
		
		math_sub_vec3(v1, v1, cam->pos);

		if(state)
		{
			if(math_norm_vec3(v1) > CAMERA_LOOSE_CHASE_LOW_THRESHOLD)
			{
				math_len_vec3(v1, v1, delta * CAMERA_LOOSE_CHASE_SPEED);
				math_add_vec3(v1, cam->pos, v1);
				render_move_camera(cam, v1[X], v1[Y], v1[Z]);
			} else
				state = 0;
		} else
			if(math_norm_vec3(v1) > CAMERA_LOOSE_CHASE_HIGH_THRESHOLD)
				state = 1;
		 
		break;
		
	case CAMERA_TYPE_LOOSE_TRAIL:
		v1[X] = cam->dir[X];
		v1[Y] = cam->dir[Y];
		v1[Z] = cam->dir[Z];

		render_point_camera(cam,
				    player->position[X],
				    player->position[Y],
				    player->position[Z]);


		v2[X] = cam->dir[X];
		v2[Y] = cam->dir[Y];
		v2[Z] = cam->dir[Z];
		
		math_sub_vec3(vr, v2, v1);
		dist = math_norm_vec3(vr);
		if(dist > CAMERA_LOOSE_TRAIL_ROTATE_SPEED * delta || state)
		{
			state = 1;

			math_len_vec3(vr, vr,
				      CAMERA_LOOSE_TRAIL_ROTATE_SPEED * delta * sin(dist));
			math_add_vec3(v1, v1, vr);
			render_point_camera(cam, v1[X], v1[Y], v1[Z]);

			math_sub_vec3(vr, player->position, cam->pos);
			if(dist * math_norm_vec3(vr) < 0.06)
				state = 0;
		}
	
		/* xx */
		v1[X] = player->position[X];
		v1[Y] = 0.0;
		v1[Z] = 0.0;

		v2[X] = cam->pos[X];
		v2[Y] = 0.0;
		v2[Z] = 0.0;

		math_sub_vec3(vr, v2, v1);

		dist = math_norm_vec3(vr);
		math_len_vec3(vr, vr,
			      MAX(-delta * player->speed * dist * 0.7,
				  -CAMERA_LOOSE_TRAIL_SPEED * delta));

		d2 = math_norm_vec3(vr);
		math_len_vec3(vr, vr, MAX(d2, d2 * (dist * 0.07) * (dist * 0.07)));

		render_translate_camera(cam, vr[X], vr[Y], vr[Z]);

		/* yy */
		v1[X] = 0.0;
		v1[Y] = CAMERA_LOOSE_TRAIL_Y_OFFSET;
		v1[Z] = 0.0;

		v2[X] = 0.0;
		v2[Y] = cam->pos[Y];
		v2[Z] = 0.0;

		math_sub_vec3(vr, v2, v1);

		dist = math_norm_vec3(vr);
		math_len_vec3(vr, vr,
			      MAX(-delta * player->speed * dist * 0.7,
				  -CAMERA_LOOSE_TRAIL_SPEED * delta));

		d2 = math_norm_vec3(vr);
		math_len_vec3(vr, vr, MAX(d2, d2 * (dist * 0.07) * (dist * 0.07)));

		render_translate_camera(cam, vr[X], vr[Y], vr[Z]);


		/* zz */
		v1[X] = 0.0;
		v1[Y] = 0.0;
		v1[Z] = player->position[Z] + CAMERA_LOOSE_TRAIL_Z_OFFSET;
		
		v2[X] = 0.0;
		v2[Y] = 0.0;
		v2[Z] = cam->pos[Z];
		
		math_sub_vec3(vr, v2, v1);

		dist = math_norm_vec3(vr);
		math_len_vec3(vr, vr,
			      MAX(-delta * player->speed * dist * 0.9,
				  -CAMERA_LOOSE_TRAIL_SPEED * delta));

		d2 = math_norm_vec3(vr);
		math_len_vec3(vr, vr, MAX(d2, d2 * (dist * 0.07) * (dist * 0.07)));

		render_translate_camera(cam, vr[X], vr[Y], vr[Z]);

		break;

	case CAMERA_TYPE_DEMO:
		v1[X] = game->map->width / 2 + 0.5;
		v1[Y] = 0.0;
		v1[Z] = game->map->height / 2 + 0.5;
		
		v2[X] = 0.0;
		v2[Y] = 1.0;
		v2[Z] = 0.0;
		
		math_sub_vec3(vr, cam->pos, v1);
		math_rotate_vec3(vr, vr, v2, CAMERA_DEMO_ROTATE_SPEED * delta);
		math_add_vec3(cam->pos, v1, vr);

		render_point_camera(cam,
				    game->map->width / 2 + 0.5, 0.0,
				    game->map->height / 2 + 0.5);
		break;
		
	case CAMERA_TYPE_TOMB_RAIDER:
		v1[X] = cam->dir[X];
		v1[Y] = cam->dir[Y];
		v1[Z] = cam->dir[Z];

		render_point_camera(cam,
				    player->position[X],
				    player->position[Y],
				    player->position[Z]);


		v2[X] = cam->dir[X];
		v2[Y] = cam->dir[Y];
		v2[Z] = cam->dir[Z];
		
		math_sub_vec3(vr, v2, v1);
		dist = math_norm_vec3(vr);
		if(dist > CAMERA_TOMB_RAIDER_ROTATE_SPEED * delta || state)
		{
			state = 1;

			math_len_vec3(vr, vr,
				      CAMERA_TOMB_RAIDER_ROTATE_SPEED * delta * sin(dist));
			math_add_vec3(v1, v1, vr);
			render_point_camera(cam, v1[X], v1[Y], v1[Z]);

			math_sub_vec3(vr, player->position, cam->pos);
			if(dist * math_norm_vec3(vr) < 0.6)
				state = 0;
		}
	
		/* xx */
		switch(player->direction)
		{
		case DIRECTION_UP:
		case DIRECTION_DOWN:
			v1[X] = player->position[X];
			break;
			
		case DIRECTION_LEFT:
			v1[X] = player->position[X] + CAMERA_TOMB_RAIDER_OFFSET;
			break;
			
		case DIRECTION_RIGHT:
			v1[X] = player->position[X] - CAMERA_TOMB_RAIDER_OFFSET;
			break;
		}
		v1[Y] = 0.0;
		v1[Z] = 0.0;
			
		v2[X] = cam->pos[X];
		v2[Y] = 0.0;
		v2[Z] = 0.0;

		math_sub_vec3(vr, v2, v1);

		dist = math_norm_vec3(vr);
		math_len_vec3(vr, vr,
			      MAX(-delta * player->speed * dist * 0.7,
				  -CAMERA_TOMB_RAIDER_SPEED * delta));

		d2 = math_norm_vec3(vr);
		math_len_vec3(vr, vr, MAX(d2, d2 * (dist * 0.07) * (dist * 0.07)));

		render_translate_camera(cam, vr[X], vr[Y], vr[Z]);

		/* yy */
		v1[X] = 0.0;
		v1[Y] = CAMERA_TOMB_RAIDER_Y_OFFSET;
		v1[Z] = 0.0;
			
		v2[X] = 0.0;
		v2[Y] = cam->pos[Y];
		v2[Z] = 0.0;

		math_sub_vec3(vr, v2, v1);

		dist = math_norm_vec3(vr);
		math_len_vec3(vr, vr,
			      MAX(-delta * player->speed * dist * 0.7,
				  -CAMERA_TOMB_RAIDER_SPEED * delta));

		d2 = math_norm_vec3(vr);
		math_len_vec3(vr, vr, MAX(d2, d2 * (dist * 0.07) * (dist * 0.07)));

		render_translate_camera(cam, vr[X], vr[Y], vr[Z]);

		/* zz */
		v1[X] = 0.0;
		v1[Y] = 0.0;
		switch(player->direction)
		{
		case DIRECTION_UP:
			v1[Z] = player->position[Z] - CAMERA_TOMB_RAIDER_OFFSET;
			break;
			
		case DIRECTION_DOWN:
			v1[Z] = player->position[Z] + CAMERA_TOMB_RAIDER_OFFSET;
			break;
			
		case DIRECTION_LEFT:
		case DIRECTION_RIGHT:
			v1[Z] = player->position[Z];
			break;
		}
		
		v2[X] = 0.0;
		v2[Y] = 0.0;
		v2[Z] = cam->pos[Z];
		
		math_sub_vec3(vr, v2, v1);

		dist = math_norm_vec3(vr);
		math_len_vec3(vr, vr,
			      MAX(-delta * player->speed * dist * 0.9,
				  -CAMERA_TOMB_RAIDER_SPEED * delta));

		d2 = math_norm_vec3(vr);
		math_len_vec3(vr, vr, MAX(d2, d2 * (dist * 0.07) * (dist * 0.07)));

		render_translate_camera(cam, vr[X], vr[Y], vr[Z]);

		break;

	case CAMERA_TYPE_FREE:
		if(input_kstate(SDLK_w))
			/* move forward */
			render_advance_camera(cam, 0.0, 0.0, 0.1);

		if(input_kstate(SDLK_s))
			/* move backward */
			render_advance_camera(cam, 0.0, 0.0, -0.1);

		if(input_kstate(SDLK_a))
			/* move left */
			render_advance_camera(cam, -0.1, 0.0, 0.0);

		if(input_kstate(SDLK_d))
			/* move right */
			render_advance_camera(cam, 0.1, 0.0, 0.0);

		if(input_kstate(SDLK_r))
			/* move up */
			render_advance_camera(cam, 0.0, -0.1, 0.0);

		if(input_kstate(SDLK_f))
			/* move down */
			render_advance_camera(cam, 0.0, 0.1, 0.0);

		if(input_kstate(SDLK_q))
			/* roll Cwise */
			render_camera_roll(cam, 1.0);

		if(input_kstate(SDLK_e))
			/* roll CCwise */
			render_camera_roll(cam, -1.0);

		if(input_kstate(SDLK_z))
			/* rotate Cwise */
			render_camera_yaw(cam, 1.0);

		if(input_kstate(SDLK_x))
			/* rotate CCwise */
			render_camera_yaw(cam, -1.0);

		if(input_kstate(SDLK_t))
			/* pitch up */
			render_camera_pitch(cam, -1.0);

		if(input_kstate(SDLK_g))
			/* pitch down */
			render_camera_pitch(cam, 1.0);

		break;
	}
}

/*
  actualiza o estado do jogo
*/
void game_update(struct game *game, float delta)
{
	int c;

	map_update(game->map, delta);

	for(c = 0; c < game->n_players; c++)
	{
		player_update(game, c, delta);
		game_update_camera(game, c, delta);
	}
	
	for(c = 0; c < game->n_ghosts; c++)
		ghost_update(game, c, delta);

	for(c = 0; c < game->n_shots; c++)
		shot_update(game, c, delta);

	for(c = 0; c < game->n_bombs; c++)
		bomb_update(game, c, delta);

	if(game->demo_camera)
		game_update_camera(game, -1, delta);
}

void game_draw_opaque_objects(struct game *game, int player_no)
{
	int c;

	for(c = 0; c < game->n_ghosts; c++)
		ghost_render(game, &game->ghosts[c]);
	
	for(c = 0; c < game->n_players; c++)
		player_render(&game->players[c]);

	for(c = 0; c < game->n_shots; c++)
		shot_render_opaque(game, c);

	for(c = 0; c < game->n_bombs; c++)
		bomb_render_opaque(game, c);
}

void game_draw_translucent_objects(struct game *game, int player_no)
{
	int c;

	for(c = 0; c < game->n_shots; c++)
		shot_render_translucent(game, player_no, c);

	for(c = 0; c < game->n_bombs; c++)
		bomb_render_translucent(game, player_no, c);
}
