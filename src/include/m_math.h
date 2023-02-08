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

/* $Id: m_math.h,v 1.2 2003/11/23 00:06:39 nsubtil Exp $ */

#ifndef _M_MATH_H
#define _M_MATH_H

#define MATH_COPY_VEC3(dest, src) dest[X] = src[X]; dest[Y] = src[Y]; dest[Z] = src[Z]
#define MATH_SET_VEC3(vec, x, y, z) vec[X] = x; vec[Y] = y; vec[Z] = z

#define MATH_COPY_VEC4(dest, src) dest[X] = src[X]; dest[Y] = src[Y]; dest[Z] = src[Z]; dest[W] = src[W]
#define MATH_SET_VEC4(vec, x, y, z, w) vec[X] = x; vec[Y] = y; vec[Z] = z; vec[W] = w

void math_sub_vec3(float r[3], float v1[3], float v2[3]);
void math_add_vec3(float r[3], float v1[3], float v2[3]);
void math_scale_vec3(float r[3], float k, float v[3]);
void math_scale_vec2(float r[2], float k, float v[2]);
void math_rotate_vec3(float ret[3], float vec[3], float axis[3], float angle);
void math_rotate_vec2(float ret[2], float vec[2], float angle);
float math_norm_vec3(float v[3]);
float math_norm_vec2(float v[2]);
void math_len_vec3(float ret[3], float v[3], float len);
void math_len_vec2(float ret[2], float v[2], float len);
float math_dot_vec2(float v1[2], float v2[2]);
float math_dot_vec3(float v1[3], float v2[3]);
float math_angle_vec2(float v1[2], float v2[2]);
float math_angle_vec3(float v1[3], float v2[3]);
int math_quadrant_vec2(float v[2]);
float math_dist_vec3(float v1[3], float v2[3]);
void math_proj_vec3(float r[3], float v[3], float u[3]);
void math_gschmidt_2vec3(float ret[3], float v1[3], float v2[3]);

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

#define DEGREES(a) a * 360.0 / (2 * M_PI)
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#ifdef _WIN32
#define acosf acos
#endif

#endif
