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
  "$Id: menu.c,v 1.26 2003/11/30 17:43:55 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <SDL.h>
#include <SDL_net.h>
#include <stdlib.h>
#include <math.h>

#include "object.h"
#include "game.h"
#include "map.h"
#include "player.h"
#include "screen.h"
#include "render.h"
#include "gfx.h"
#include "audio.h"
#include "net.h"
#include "input.h"
#include "render_map.h"

#include "menu.h"

#include "level-demo.h"
#include "level-classic.h"
#include "level-1.h"

#include "config.h"

void menu_setup_map(struct map *map)
{
	int x, y;
	
	for(y = 0; y < map->height; y++)
		for(x = 0; x < map->width; x++)
		{
			if(MAP(map, x, y).content == MAP_CONTENT_FOOD)
			{
				MAP_FOOD(map, x, y).type = FOOD_TYPE_INTERMITTENT;
				MAP_FOOD(map, x, y).status = FOOD_STATUS_EATEN;
				MAP_FOOD(map, x, y).time = sqrt(pow(x, 2) + pow(y, 2)) / 5.0;
				MAP_FOOD(map, x, y).active_time = 10.0;
				MAP_FOOD(map, x, y).fade_out = 1.0;
				MAP_FOOD(map, x, y).inactive_time = 3.0;
				MAP_FOOD(map, x, y).fade_in = 1.0;
			}
		}
}

void menu_run(void)
{
	struct game *game;
	struct menu_entry main_menu[] = 
		{
			{ "gfx/start-game-high.tga", "gfx/start-game-low.tga", NULL, NULL },
#ifndef NO_RESOLUTION_OPTION
			{ "gfx/resolution-high.tga", "gfx/resolution-low.tga", NULL, NULL },
#endif
			{ "gfx/exit-high.tga", "gfx/exit-low.tga", NULL, NULL }
		};
#ifndef NO_RESOLUTION_OPTION
	int main_menu_num = 3;
#else
	int main_menu_num = 3-1;
#endif
	int c;

	for(c = 0; c < main_menu_num; c++)
	{
		main_menu[c].image_high = gfx_get(main_menu[c].image_name_high);
		if(main_menu[c].image_high->id == -1)
		{
			gfx_alpha_from_key(main_menu[c].image_name_high, 0, 0, 0);
			gfx_upload_texture(main_menu[c].image_name_high);
		}

		main_menu[c].image_low = gfx_get(main_menu[c].image_name_low);
		if(main_menu[c].image_low->id == -1)
		{
			gfx_alpha_from_key(main_menu[c].image_name_low, 0, 0, 0);
			gfx_upload_texture(main_menu[c].image_name_low);
		}
	}

	/* preload */
	game = game_new(GAME_TYPE_DEMO);
	game->map = map_load_from_ascii((char **)level_demo, NULL,
					LEVEL_DEMO_WIDTH, LEVEL_DEMO_HEIGHT);
	menu_setup_map(game->map);

	game_reset(game);
//	game_load_level(LEVEL_DEMO);
	game->demo_camera = (struct camera *)malloc(sizeof(struct camera));
	game->demo_camera->type = CAMERA_TYPE_DEMO;
	game_reset_camera(game, -1);
	game_start(game);

	audio_play_music("sfx/hummingbird.ogg");
	screen_set_fadein(0.5);

	for(;;)
	{
		struct game *gm;
		float delta;
		int last_update, ticks;

		switch(menu(game, main_menu, main_menu_num))
		{
		case 0:

			last_update = SDL_GetTicks();
			delta = 0.0;

			audio_fade_music(500);
			screen_set_fadeout(0.5);

			for(;;)
			{
				int diff;

				ticks = SDL_GetTicks();
				diff = ticks - last_update;
				last_update = ticks;
				delta += (float)diff / 1000;

				if(delta >= 0.5)
					break;

//				screen_update_gamma((float)diff / 1000);
				render_start_frame();
				render_setup(game, -1);
				map_render_opaque_objects(game);
				game_draw_opaque_objects(game, -1);
				game_draw_translucent_objects(game, -1);
				map_render_translucent_objects(game, -1);
			}

			/* classic single-player */
			gm = game_new(GAME_TYPE_CLASSIC);
			gm->map = map_load_from_ascii((char **)level_classic,
						      (char **)level_classic_ghost_map,
						      LEVEL_CLASSIC_WIDTH, LEVEL_CLASSIC_HEIGHT);
			player_add_new(gm);
//			player_add_new(gm);
//			screen_set_num_viewports(2);
//			screen_reset_gamma();
			game_run(gm);

			screen_set_num_viewports(1);
			audio_play_music("sfx/hummingbird.ogg");
			break;

#ifdef NO_RESOLUTION_OPTION
		case 1:
			SDL_Quit();
			exit(0);
			break;
#else
		case 1:
			menu_resolution(game);
			break;
		case 2:
			SDL_Quit();
			exit(0);
			break;
#endif
		case -1:
			SDL_Quit();
			exit(0);
		}
	}
}

void menu_resolution(struct game *game)
{
	struct menu_entry menu_resolution[] = 
		{
			{ "gfx/320-240-high.tga", "gfx/320-240-low.tga", NULL, NULL },
			{ "gfx/640-480-high.tga", "gfx/640-480-low.tga", NULL, NULL },
			{ "gfx/800-600-high.tga", "gfx/800-600-low.tga", NULL, NULL },
			{ "gfx/1024-768-high.tga", "gfx/1024-768-low.tga", NULL, NULL },
			{ "gfx/1400-1050-high.tga", "gfx/1400-1050-low.tga", NULL, NULL }
		};
	int menu_resolution_num = 5;
	int c, new_w = 0, new_h = 0;
	
	for(c = 0; c < menu_resolution_num; c++)
	{
		menu_resolution[c].image_high = gfx_get(menu_resolution[c].image_name_high);
		if(menu_resolution[c].image_high->id == -1)
		{
			gfx_alpha_from_key(menu_resolution[c].image_name_high, 0, 0, 0);
			gfx_upload_texture(menu_resolution[c].image_name_high);
		}

		menu_resolution[c].image_low = gfx_get(menu_resolution[c].image_name_low);
		if(menu_resolution[c].image_low->id == -1)
		{
			gfx_alpha_from_key(menu_resolution[c].image_name_low, 0, 0, 0);
			gfx_upload_texture(menu_resolution[c].image_name_low);
		}
	}

	switch(menu(game, menu_resolution, menu_resolution_num))
	{
	case 0:
		new_w = 320;
		new_h = 240;
		break;
		
	case 1:
		new_w = 640;
		new_h = 480;
		break;
		
	case 2:
		new_w = 800;
		new_h = 600;
		break;
		
	case 3:
		new_w = 1024;
		new_h = 768;
		break;
		
	case 4:
		new_w = 1400;
		new_h = 1050;
		break;

	case -1:
		return;
		break;
	}

	screen_set_resolution(new_w, new_h);
	screen_switch_resolution();
}

int menu(struct game *game, struct menu_entry *m, int n)
{
	int last_update, c;
	int selected = 0;
	struct image_rgba32 *banner;
	struct map *map;
	struct viewport *vp;
	
	map = game->map;
	
	banner = gfx_get("gfx/pacman-arena-banner.tga");
	if(banner->id == -1)
	{
		gfx_alpha_from_key("gfx/pacman-arena-banner.tga", 0, 0, 0);
		gfx_upload_texture("gfx/pacman-arena-banner.tga");
	}

/*	
	game_reset();
	game_load_level(LEVEL_DEMO);
	camera_type = CAMERA_TYPE_DEMO;
	game_reset_camera();
	game_start();
*/

	vp = screen_get_viewport(0);
	
	last_update = SDL_GetTicks();
	
	for(;;)
	{
		int diff, top, left, width, height;

		input_update();

		if(input_kstate(SDLK_ESCAPE))
		{
			input_kclear(SDLK_ESCAPE);
			return -1;
		}

		if(input_kstate(SDLK_RETURN))
		{
			input_kclear(SDLK_RETURN);
			return selected;
		}
		
		
		if(input_kstate(SDLK_DOWN))
		{
			input_kclear(SDLK_DOWN);
			selected = (selected + 1) % n;
		}
		
		
		if(input_kstate(SDLK_UP))
		{
			input_kclear(SDLK_UP);
			selected = (selected + n - 1) % n;
		}
		
		diff = SDL_GetTicks() - last_update;
		if(server_running)
			net_server_update();

		game_update(game, (float)diff / 1000);
		last_update = SDL_GetTicks();

//		screen_update_gamma((float)diff / 1000);
		render_start_frame();
		render_setup(game, -1);
		map_render_opaque_objects(game);
		game_draw_opaque_objects(game, -1);
		game_draw_translucent_objects(game, -1);
		map_render_translucent_objects(game, -1);

		render_setup_2d(vp);

		top = vp->height / 2 - vp->height / 5;
		
		width = 0;
		height = 0;
		
		for(c = 0; c < n; c++)
		{
			if(c == selected)
			{
				if(m[c].image_high->width > width)
					width = m[c].image_high->width;

				height += m[c].image_high->height + 20;
				
			} else {
				if(m[c].image_low->width > width)
					width = m[c].image_low->width;

				height += m[c].image_low->height + 20;
			}
		}

		width += 40;
		left = vp->width / 2 - width / 2;
		
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.0, 0.0, 0.0, 0.5);
		
		glBegin(GL_QUADS);
		glVertex2i(left, top);
		glVertex2i(left + width, top);
		glVertex2i(left + width, top + height);
		glVertex2i(left, top + height);
		glEnd();

		for(c = 0; c < n; c++)
		{
			if(c == selected)
			{
				render_draw_scaled_image(
					m[c].image_name_high,
					vp->width / 2 - m[c].image_high->width / 2,
					top,
					vp->width / 2 + m[c].image_high->width / 2,
					top + m[c].image_high->height);
				
				top += m[c].image_high->height + 20;
				height += m[c].image_high->height + 20;
			} else {
				render_draw_scaled_image(
					m[c].image_name_low,
					vp->width / 2 - m[c].image_low->width / 2,
					top,
					vp->width / 2 + m[c].image_low->width / 2,
					top + m[c].image_low->height);
				
				top += m[c].image_low->height + 20;
				height += m[c].image_high->height + 20;
			}
		}


		render_draw_scaled_image("gfx/pacman-arena-banner.tga", 0, 0,
					 vp->width, vp->width * banner->height / banner->width);

		render_finish_frame();
	}
}
