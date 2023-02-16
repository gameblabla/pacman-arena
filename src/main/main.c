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
  "$Id: main.c,v 1.60 2003/11/27 22:11:57 nsubtil Exp $";
  
#include "common.h"

#ifdef PSP2
int _newlib_heap_size_user = 256 * 1024 * 1024;
#endif

#include "audio.h"
#include "object.h"
#include "game.h"
#include "map.h"
#include "player.h"
#include "render.h"
#include "m_math.h"
#include "menu.h"
#include "screen.h"
#include "net.h"

int main(int argc, char **argv)
{
/*
	render_reshape_window(screen->w, screen->h);
*/

	screen_init();
	srand(SDL_GetTicks()); // Do it after SDL has been initiliazed
	audio_init();
	
	load_map_assets();

	/* preload */
/*
	object_read_file("gfx/pacman-moving.3d", &last_update);
	object_read_file("gfx/pacman-dying.3d", &last_update);
	object_read_file("gfx/pacman-stopped.3d", &last_update);
	object_read_file("gfx/ghost-green-moving.3d", &last_update);
	object_read_file("gfx/ghost-green-dying.3d", &last_update);
	object_read_file("gfx/ghost-green-returning.3d", &last_update);
*/

#ifdef NETWORKING_GAME
	if(argc > 1 && strcmp(argv[1], "--server") == 0)
		net_server_init();

	if(argc > 1 && strcmp(argv[1], "--connect") == 0)
		net_client_init(argv[2]);
#endif

	for(;;)
		menu_run();

	return 0;
}
