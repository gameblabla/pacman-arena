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

/* $Id: level-classic.h,v 1.1 2003/11/22 16:22:08 nsubtil Exp $ */

#ifndef _LEVEL_CLASSIC_H
#define _LEVEL_CLASSIC_H

/*
  pacman "clássico"
  (galaxian hardware bootleg)
*/

#define LEVEL_CLASSIC_WIDTH 30
#define LEVEL_CLASSIC_HEIGHT 31

const char *level_classic[] = {
	"L------------RNNL------------R",
	"|*...........|NN|...........*|",
	"|.L--R.L---R.|NN|.L---R.L--R.|",
	"|.|NN|.|NNN|.|NN|.|NNN|.|NN|.|",
	"|.l--r.l---r.l--r.l---r.l--r.|",
	"|............................|",
	"|.L--R.LR.L--------R.LR.L--R.|",
	"|.|NN|.||.l--RNNL--r.||.|NN|.|",
	"|.l--r.||....|NN|....||.l--r.|",
	"|......|l--R.|NN|.L--r|......|",
	"l----R.|L--r.l--r.l--R|.L----r",
	"NNNNN|.||............||.|NNNNN",
	"NNNNN|.||.L--RUL---R.||.|NNNNN",
	"-----r.lr.|L-rUl--R|.lr.l-----",
	"0.........||GGUGNG||.........1",
	"-----R.LR.|l------r|.LR.L-----",
	"NNNNN|.||.l--------r.||.|NNNNN",
	"NNNNN|.||............||.|NNNNN",
	"L----r.||.L--------R.||.l----R",
	"|*.....lr.l---RL---r.lr.....*|",
	"|.L--R........||........L--R.|",
	"|.|NN|.L----R.||.L----R.|NN|.|",
	"|.l-R|.l----r.lr.l----r.|L-r.|",
	"|...||........S.........||...|",
	"l-R.||.LR.L--------R.LR.||.L-r",
	"L-r.lr.||.l--RNL---r.||.lr.l-R",
	"|......||....|N|S....||......|",
	"|.L----rl--R.|N|.L---rl----R.|",
	"|.l--------r.l-r.l---------r.|",
	"|*..........................*|",
	"l----------------------------r"
};

const char *level_classic_ghost_map[] = {
	"                              ",
	" RRRRRDLLLRRD    DLLLRRDLLLLL ",
	" U    D     D    D     D    U ",
	" D    D     D    D     D    D ",
	" D    D     D    D     D    D ",
	" RRRRRRRRDLLLLLRRRRRDLRDLLLLL ",
	" U    U  D          D  D    U ",
	" D    U  D          D  D    D ",
	" D    U  RRRD    DLLL  D    D ",
	" RRRRRU     D    D     DLLLLL ",
	"      D     D    D     D      ",
	"      D  RRRRRDLLLLLL  D      ",
	"      D  U    D     U  D      ",
	"      D  U    D     U  D      ",
	" RRRRRRRRU  G RG G  ULLLLLLLL ",
	"      U  U          U  U      ",
	"      U  U          U  U      ",
	"      U  ULLLLLRRRRRU  D      ",
	"      U  U          U  D      ",
	" RRRRRD  U          U  DLLLLL ",
	" U    RRRULLLL  RRRRULLL    U ",
	" U    U      U  U      U    U ",
	" U    U      U  U      U    U ",
	" ULU  ULLLRRRULRULLLLRRU  RRU ",
	"   U  U  U          U  U  U   ",
	"   U  U  U          U  U  U   ",
	" RRRRRU  ULLL   RRRRU  ULLLLL ",
	" U          U   U           U ",
	" U          U   U           U ",
	" ULLLLLLRRRRULLRULLLLRRRRRRRU ",
	"                              "
};

#endif

