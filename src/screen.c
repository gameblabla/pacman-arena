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
  "$Id: screen.c,v 1.7 2003/11/30 17:43:55 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL.h>
#include <GL/gl.h>

#include <stdlib.h>
#include <assert.h>

#include "object.h"
#include "gfx.h"

#include "screen.h"

#include "config.h"

#define GAMMA_DIR_FADEIN 1
#define GAMMA_DIR_FADEOUT 2

static struct screen scr;
static float cur_gamma, target_gamma;
static int gamma_direction;
static float last_gamma;

/*
  initializes the screen
*/
void screen_init(void)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE);
	/* XXX - startup resolutions */
	atexit(SDL_Quit);
	
	scr.width = SCREEN_WIDTH_GAME;
	scr.height = SCREEN_HEIGHT_GAME;
	scr.bpp = SCREEN_BITDEPTH_GAME;
	#ifdef NO_FULLSCREEN
	scr.fullscreen = 0;
	#else
	scr.fullscreen = SDL_FULLSCREEN;
	#endif
	scr.surface = NULL;
	scr.viewports = NULL;
	scr.cur_viewport = 0;

	scr.num_viewports = 1;

	screen_switch_resolution();

	SDL_WM_SetCaption("Pacman Arena", "Pacman Arena");
	SDL_ShowCursor(SDL_DISABLE);

	printf("%s\n", glGetString(GL_RENDERER));
	printf("%s\n", glGetString(GL_VENDOR));
}

/*
  attempts to setup the screen according to scr
*/
void screen_switch_resolution(void)
{
	gfx_unload_all();
	object_release_dlists();
	
	scr.surface = SDL_SetVideoMode(scr.width, scr.height, scr.bpp, SDL_OPENGL | scr.fullscreen);
	if(scr.surface == NULL)
	{
		printf("screen_switch_resolution: couldn't set mode (%dx%dx%d %s)\n",
		       scr.width, scr.height, scr.bpp, scr.fullscreen ? "fullscreen" : "windowed");
		SDL_Quit();
		exit(1);
	}

	screen_setup_viewports();

	gfx_reload_all();
	object_recompile_dlists();
}

/*
  sets a new resolution
*/
void screen_set_resolution(int w, int h)
{
	scr.width = w;
	scr.height = h;
}

/*
  sets a new bpp
*/
void screen_set_bpp(int bpp)
{
	scr.bpp = bpp;
}

/*
  toggles full-screen mode
*/
void screen_toggle_fullscreen(void)
{
	if(scr.fullscreen)
		scr.fullscreen = 0;
	else
		scr.fullscreen = SDL_FULLSCREEN;
	
	screen_switch_resolution();
}

/*
  sets the number of viewports
*/
void screen_set_num_viewports(int n)
{
	scr.num_viewports = n;
	screen_setup_viewports();
}

/*
  sets up viewports for scr
  XXX - should handle any number of viewports ?
*/
void screen_setup_viewports(void)
{
	struct viewport *vp;
	
	if(scr.viewports)
		free(scr.viewports);

	scr.viewports = malloc(sizeof(struct viewport) * scr.num_viewports);

	switch(scr.num_viewports)
	{
	case 1:
		/* one viewport, whole screen */
		vp = &scr.viewports[0];

		vp->ll_x = 0;
		vp->ll_y = 0;
		
		vp->width = scr.width;
		vp->height = scr.height;
		
		break;
		
	case 2:
		/* two viewports, horizontal split */
		vp = &scr.viewports[0];
		vp->ll_x = 0;
		vp->ll_y = scr.height / 2;
		vp->width = scr.width;
		vp->height = scr.height / 2;
		
		vp = &scr.viewports[1];
		vp->ll_x = 0;
		vp->ll_y = 0;
		vp->width = scr.width;
		vp->height = scr.height / 2;
		
		break;
		
	default:
		printf("screen_setup_viewports: too many viewports (%d)\n", scr.num_viewports);
		SDL_Quit();
		exit(1);
		
		break;
	}
}

/*
  sets the active opengl viewport
*/
void screen_set_active_viewport(int vp)
{
	assert(vp >= 0 && vp < scr.num_viewports);
	glViewport(scr.viewports[vp].ll_x, scr.viewports[vp].ll_y,
		   scr.viewports[vp].width, scr.viewports[vp].height);
}

/*
  returns a viewport structure
*/
struct viewport *screen_get_viewport(int vp)
{
	assert(vp >= 0 && vp < scr.num_viewports);
	return &scr.viewports[vp];
}

/*
  fades in for X seconds
*/
void screen_set_fadein(float time)
{
	cur_gamma = 0.0;
	target_gamma = time;
	gamma_direction = GAMMA_DIR_FADEIN;
}

/*
  fades out for X seconds
*/
void screen_set_fadeout(float time)
{
	cur_gamma = 0.0;
	target_gamma = time;
	gamma_direction = GAMMA_DIR_FADEOUT;
}

void screen_update_gamma(float delta)
{
	float gamma;
	
	cur_gamma += delta;
	if(cur_gamma > target_gamma)
		cur_gamma = target_gamma;

	gamma = cur_gamma / target_gamma;
	if(gamma_direction == GAMMA_DIR_FADEOUT)
		gamma = 1.0 - gamma;

	if(last_gamma != gamma)
	{
		SDL_SetGamma(gamma, gamma, gamma);
		last_gamma = gamma;
	}
}

void screen_reset_gamma(void)
{
	cur_gamma = target_gamma = last_gamma = 1.0;
	SDL_SetGamma(cur_gamma, cur_gamma, cur_gamma);
}
