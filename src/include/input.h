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

/* $Id: input.h,v 1.1 2003/11/22 16:22:08 nsubtil Exp $ */

#ifndef _INPUT_H
#define _INPUT_H

#include <SDL.h>

extern char keyboard_map[];

void input_reset(void);
void input_update(void);
int input_kstate(int ksym);
void input_kclear(int ksym);

#endif
