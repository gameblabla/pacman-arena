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
  "$Id: input.c,v 1.4 2003/11/30 17:43:55 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#include "screen.h"

#ifdef SDL2
#define SDLK_LAST SDLK_ENDCALL
char keyboard_map[SDLK_ENDCALL];
#else
char keyboard_map[SDLK_LAST];
#endif

void input_reset(void)
{
	memset(keyboard_map, 0, sizeof(keyboard_map));
}

/*
  processes all events and clears the SDL event queue
 */
void input_update(void)
{
	SDL_Event ev;
	
	while(SDL_PollEvent(&ev))
	{
		if(ev.type == SDL_QUIT)
		{
			SDL_Quit();
			exit(0);
		}

		if(ev.type == SDL_KEYDOWN)
		{
			keyboard_map[ev.key.keysym.sym] = 1;

			/* special hooks */
			if(keyboard_map[SDLK_LALT] && keyboard_map[SDLK_RETURN])
			{
				screen_toggle_fullscreen();
				keyboard_map[SDLK_LALT] = 0;
				keyboard_map[SDLK_RETURN] = 0;
			}
		}
		
		if(ev.type == SDL_KEYUP)
			keyboard_map[ev.key.keysym.sym] = 0;
	}
}

/*
  returns the current state of a key
*/
int input_kstate(int ksym)
{
	if(ksym < 0 || ksym >= SDLK_LAST)
	{
		printf("input_kstate: invalid ksym %d\n", ksym);
		SDL_Quit();
		exit(1);
	}
	
	return keyboard_map[ksym];
}

/*
  clears a key state
*/
void input_kclear(int ksym)
{
	if(ksym < 0 || ksym >= SDLK_LAST)
	{
		printf("input_kclear: invalid ksym %d\n", ksym);
		SDL_Quit();
		exit(1);
	}
	
	keyboard_map[ksym] = 0;
}
