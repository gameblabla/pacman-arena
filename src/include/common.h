#ifndef COMMON_H
#define COMMON_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <SDL.h>
#include <SDL_mixer.h>
#ifdef NETWORKING_GAME
#include <SDL_net.h>
#endif

#ifdef PSP2
#include <vitaGL.h>
#include "vita_stubs.h"
#else
#include <SDL_video.h>
#include <GL/gl.h>
#endif

extern void mygluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz);


#endif
