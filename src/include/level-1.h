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

/* $Id: level-1.h,v 1.1 2003/11/22 16:22:08 nsubtil Exp $ */

#ifndef _LEVEL_1_H
#define _LEVEL_1_H

#define LEVEL_1_WIDTH 31
#define LEVEL_1_HEIGHT 39

const char *level_1[] = {
	"L-----------------------------R",
	"|*...........................*|",
	"|.L--R.L--R.L-----R.L--R.L--R.|",
	"|.|NN|.|NN|.|L---R|.|NN|.|NN|.|",
	"|.|NN|.|NN|.|lRGLr|.|NN|.|NN|.|",
	"|.l--r.l--r.l-rNl-r.l--r.l--r.|",
	"|..............*..............|",
	"|.L--R.L--R.L-----R.L--R.L--R.|",
	"|.|NN|.|NN|.|L---R|.|NN|.|NN|.|",
	"|.|NN|.|NN|.|lRGLr|.|NN|.|NN|.|",
	"|.l--r.l--r.l-rNl-r.l--r.l--r.|",
	"|*...........................*|",
	"|.L-------------------------R.|",
	"|.|L-----------------------R|.|",
	"|.||.......................||.|",
	"|.|l----------R.L----------r|.|",
	"|.l-----------r.l-----------r.|",
	"|..............*..............|",
	"|.L--R.L--R.L-RNL-R.L--R.L--R.|",
	"|.|NN|.|NN|.|LrGlR|.|NN|.|NN|.|",
	"|.|NN|.|NN|.|l---r|.|NN|.|NN|.|",
	"|.l--r.l--r.l-----r.l--r.l--r.|",
	"|*...........................*|",
	"|.L--R.L--R.L-RNL-R.L--R.L--R.|",
	"|.|NN|.|NN|.|LrGlR|.|NN|.|NN|.|",
	"|.|NN|.|NN|.|l---r|.|NN|.|NN|.|",
	"|.l--r.l--r.l-----r.l--r.l--r.|",
	"|..............*..............|",
	"|.L--R.L--R.L-RNL-R.L--R.L--R.|",
	"|.|NN|.|NN|.|LrGlR|.|NN|.|NN|.|",
	"|.|NN|.|NN|.|l---r|.|NN|.|NN|.|",
	"|.l--r.l--r.l-----r.l--r.l--r.|",
	"|*...........................*|",
	"|.L--R.L--R.L-RNL-R.L--R.L--R.|",
	"|.|NN|.|NN|.|LrGlR|.|NN|.|NN|.|",
	"|.|NN|.|NN|.|l---r|.|NN|.|NN|.|",
	"|.l--r.l--r.l-----r.l--r.l--r.|",
	"|*.............S.............*|",
	"l-----------------------------r"
};

const char *level_1_ghost_map[] = {
	"L-----------------------------R",
	"|DLLRRDLLRRDLLLRRRRDLLRRDLLRRD|",
	"|DL--RUL--RDL-----RDL--RDL--RD|",
	"|D|NN|U|NN|D|L---R|D|NN|D|NN|D|",
	"|D|NN|D|NN|D|lRGLr|D|NN|D|NN|D|",
	"|Dl--rDl--rDl-rUl-rDl--rDl--rD|",
	"|RRRRRRRRRRRRRRULLLLLLLLLLLLLL|",
	"|UL--RUL--RUL-----RUL--RUL--RU|",
	"|U|NN|U|NN|U|L---R|U|NN|U|NN|U|",
	"|D|NN|D|NN|D|lRGLr|D|NN|D|NN|D|",
	"|Dl--rDl--rDl-rUl-rDl--rDl--rD|",
	"|RRRRRRRRRRRRRRULLLLLLLLLLLLLL|",
	"|UL-------------------------RU|",
	"|U|L-----------------------R|U|",
	"|D||RRRRRRRRRRRDLLLLLLLLLLL||D|",
	"|D|l----------RDL----------r|D|",
	"|Dl-----------rDl-----------rD|",
	"|RRRRRRRRRRRRRRDLLLLLLLLLLLLLL|",
	"|UL--RUL--RUL-RDL-RUL--RUL--RU|",
	"|U|NN|U|NN|U|LrGlR|U|NN|U|NN|U|",
	"|D|NN|D|NN|D|l---r|D|NN|D|NN|D|",
	"|Dl--rDl--rDl-----rDl--rDl--rD|",
	"|RRRRRRRRRRRRRRDLLLLLLLLLLLLLL|",
	"|UL--RUL--RUL-RDL-RUL--RUL--RU|",
	"|U|NN|U|NN|U|LrGlR|U|NN|U|NN|U|",
	"|D|NN|D|NN|D|l---r|D|NN|D|NN|D|",
	"|Dl--rDl--rDl-----rDl--rDl--rD|",
	"|RRRRRRRRRRRRRRDLLLLLLLLLLLLLL|",
	"|UL--RUL--RUL-RDL-RUL--RUL--RU|",
	"|U|NN|U|NN|U|LrGlR|U|NN|U|NN|U|",
	"|D|NN|D|NN|D|l---r|D|NN|D|NN|D|",
	"|Dl--rDl--rDl-----rDl--rDl--rD|",
	"|RRRRRRRRRRRRRRDLLLLLLLLLLLLLL|",
	"|UL--RUL--RUL-RDL-RUL--RUL--RU|",
	"|U|NN|U|NN|U|LrGlR|U|NN|U|NN|U|",
	"|D|NN|D|NN|D|l---r|D|NN|D|NN|D|",
	"|Dl--rDl--rDl-----rDl--rDl--rD|",
	"|RRRRRRRRRRRRRRDLLLLLLLLLLLLLL|",
	"l-----------------------------r"
};

#endif

