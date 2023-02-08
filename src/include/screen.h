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

/* $Id: screen.h,v 1.3 2003/11/30 17:43:55 nsubtil Exp $ */

#ifndef _SCREEN_H
#define _SCREEN_H

struct viewport
{
	int ll_x, ll_y;	/* lower left corner */
	int width, height;
};

struct screen
{
	/* viewports */
	int num_viewports;
	struct viewport *viewports;
	int cur_viewport;

	/* physical screen */
	int width, height, bpp;
	int fullscreen;
	SDL_Surface *surface;
};

void screen_init(void);
void screen_switch_resolution(void);
void screen_set_resolution(int w, int h);
void screen_set_bpp(int bpp);
void screen_toggle_fullscreen(void);
void screen_set_num_viewports(int n);
void screen_setup_viewports(void);
void screen_set_active_viewport(int vp);
struct viewport *screen_get_viewport(int vp);
void screen_set_fadein(float time);
void screen_set_fadeout(float time);
void screen_update_gamma(float delta);

#endif
