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

/* $Id: audio.h,v 1.2 2003/11/22 17:32:10 nsubtil Exp $ */

#ifndef _AUDIO_H
#define _AUDIO_H

void audio_init(void);
void audio_play_music(char *fname);
void audio_pause_music(void);
void audio_resume_music(void);
void audio_stop_music(void);
void audio_fade_music(int msec);
struct audio_sample *audio_find_sample(char *name);
void audio_play_sample(char *fname);

#endif
