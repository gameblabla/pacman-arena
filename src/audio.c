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
  "$Id: audio.c,v 1.7 2003/11/22 17:32:09 nsubtil Exp $";

#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio.h"
#include "linked-lists.h"
#include "file.h"

/* nº de canais de som */
#define NUM_CHANNELS 8

static Mix_Music *music;

struct audio_sample
{
	char *name;
	Mix_Chunk *sample;

	struct audio_sample *next;
};

static struct audio_sample *samples = NULL;

static int audio_mute_samples = 0;
static int audio_mute_music = 0;

/*
  audio_init
  inicializa as rotinas de áudio
*/
void audio_init(void)
{
#ifndef NOSOUND
	if(Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512 * 4) == -1)
	{
		printf("erro: audio_init: Mix_OpenAudio() -> %s\n", Mix_GetError());
		SDL_Quit();
		exit(1);
	}

	Mix_AllocateChannels(NUM_CHANNELS);
#endif
}

/*
  audio_play_music
  inicia a reprodução de música de um ficheiro
*/
void audio_play_music(char *fname)
{
#ifndef NOSOUND
	if(audio_mute_music)
		return;
	
	music = Mix_LoadMUS((const char *)file_make_fname(fname));
	if(music == NULL)
	{
		printf("erro: audio_play_music: Mix_LoadMUS() -> %s\n", Mix_GetError());
		SDL_Quit();
		exit(1);
	}
	
	if(Mix_PlayMusic(music, -1) == -1)
	{
		printf("erro: audio_play_music: Mix_PlayMusic() -> %s\n", Mix_GetError());
		SDL_Quit();
		exit(1);
	}
#endif
}

/*
  audio_pause_music
  suspende a reprodução de música
*/
void audio_pause_music(void)
{
#ifndef NOSOUND
	Mix_PauseMusic();
#endif
}

/*
  audio_resume_music
  continua a reprodução de música
*/
void audio_resume_music(void)
{
#ifndef NOSOUND
	Mix_ResumeMusic();
#endif
}

/*
  audio_stop_music
  termina a reprodução de música
*/
void audio_stop_music(void)
{
#ifndef NOSOUND
	Mix_HaltMusic();
#endif
}

void audio_fade_music(int msec)
{
#ifndef NOSOUND
	Mix_FadeOutMusic(msec);
#endif
}

/*
  audio_find_sample
  procura um sample de som pelo seu nome na lista
*/
struct audio_sample *audio_find_sample(char *name)
{
#ifndef NOSOUND
	struct audio_sample *cur;
	
	cur = samples;
	while(cur)
	{
		if(strcmp(cur->name, name) == 0)
			return cur;
		
		cur = cur->next;
	}
#endif
	return NULL;

}

/*
  audio_play_sample
  reproduz um sample de som, carregando-o caso necessário
*/
void audio_play_sample(char *name)
{
#ifndef NOSOUND
	struct audio_sample *sm;
	
	sm = audio_find_sample(name);
	if(sm == NULL)
	{
		/* carregar sample do ficheiro */
		sm = malloc(sizeof(struct audio_sample));
		sm->name = strdup(name);
		sm->sample = Mix_LoadWAV((const char *)file_make_fname(name));
		if(sm->sample == NULL)
		{
			printf("erro: audio_play_sample(%s): Mix_LoadWAV() -> %s\n",
			       name, Mix_GetError());
			SDL_Quit();
			exit(1);
		}

		LLIST_ADD(struct audio_sample, samples, sm);
	}

	if(audio_mute_samples)
		return;
	
	Mix_PlayChannel(-1, sm->sample, 0);
#endif
}
