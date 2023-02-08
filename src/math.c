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
  "$Id: math.c,v 1.13 2003/11/22 17:32:09 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <stdio.h>
#include <math.h>

#include "object.h"
#include "m_math.h"

/*
  subtrai dois vectores (/r = /v1 - /v2)
*/
void math_sub_vec3(float r[3], float v1[3], float v2[3])
{
	r[X] = v1[X] - v2[X];
	r[Y] = v1[Y] - v2[Y];
	r[Z] = v1[Z] - v2[Z];
}

/*
  soma dois vectores (/r = /v1 + /v2)
*/
void math_add_vec3(float r[3], float v1[3], float v2[3])
{
	r[X] = v1[X] + v2[X];
	r[Y] = v1[Y] + v2[Y];
	r[Z] = v1[Z] + v2[Z];
}

/*
  multiplica um vector por um escalar (/r = k * /v)
*/
void math_scale_vec3(float r[3], float k, float v[3])
{
	r[X] = k * v[X];
	r[Y] = k * v[Y];
	r[Z] = k * v[Z];
}

void math_scale_vec2(float r[2], float k, float v[2])
{
	r[X] = k * v[X];
	r[Y] = k * v[Y];
}

/*
  roda vec em torno de axis
*/
void math_rotate_vec3(float ret[3], float vec[3], float axis[3], float angle)
{
	float matrix[4*4];
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	
	glRotatef(angle, axis[X], axis[Y], axis[Z]);

	matrix[12] = vec[X];
	matrix[13] = vec[Y];
	matrix[14] = vec[Z];
	
	glMultMatrixf(matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	
	ret[X] = matrix[12];
	ret[Y] = matrix[13];
	ret[Z] = matrix[14];

	glPopMatrix();
}

/*
  roda um vector 2d em torno da origem
*/
void math_rotate_vec2(float ret[2], float vec[2], float angle)
{
	float matrix[4*4];

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	
	glRotatef(angle, 0.0, -1.0, 0.0);

	matrix[12] = vec[X];
	matrix[13] = 0.0;
	matrix[14] = vec[Y];
	
	glMultMatrixf(matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	
	ret[X] = matrix[12];
	ret[Y] = matrix[14];

	glPopMatrix();
}

	
/*
  calcula a norma de um vector
*/
float math_norm_vec3(float v[3])
{
	return sqrt(v[X] * v[X] + v[Y] * v[Y] + v[Z] * v[Z]);
}

float math_norm_vec2(float v[2])
{
	return sqrt(v[X] * v[X] + v[Y] * v[Y]);
}

/*
  altera a norma de um vector para um valor definido
*/
void math_len_vec3(float ret[3], float v[3], float len)
{
	if(len == 0.0)
	{
		ret[X] = 0.0;
		ret[Y] = 0.0;
		ret[Z] = 0.0;
		return;
	}

	math_scale_vec3(ret, 1.0 / (math_norm_vec3(v) / len), v);
}

void math_len_vec2(float ret[2], float v[2], float len)
{
	math_scale_vec2(ret, 1.0 / (math_norm_vec2(v) / len), v);
}

/*
  produto escalar de dois vectores
*/
float math_dot_vec2(float v1[2], float v2[2])
{
	return v1[X] * v2[X] + v1[Y] * v2[Y];
}

float math_dot_vec3(float v1[3], float v2[3])
{
	return v1[X] * v2[X] + v1[Y] * v2[Y] + v1[Z] * v2[Z];
}

/*
  menor ângulo entre dois vectores
*/
float math_angle_vec2(float v1[2], float v2[2])
{
	return acosf(math_dot_vec2(v1, v2) / (math_norm_vec2(v1) * math_norm_vec2(v2)));
}

float math_angle_vec3(float v1[2], float v2[2])
{
	return acosf(math_dot_vec3(v1, v2) / (math_norm_vec3(v1) * math_norm_vec3(v2)));
}

/*
  devolve o quadrante de um vector 2d
*/
int math_quadrant_vec2(float v[2])
{
	if(v[X] >= 0.0 && v[Y] >= 0.0)
		/* 1o quadrante */
		return 1;

	if(v[X] < 0.0 && v[Y] >= 0.0)
		/* 2o quadrante */
		return 2;

	if(v[X] < 0.0 && v[Y] < 0.0)
		/* 3o quadrante */
		return 3;

	return 4;
}

/*
  distância entre dois pontos
*/
float math_dist_vec3(float v1[3], float v2[3])
{
	float tmp[3];

	math_sub_vec3(tmp, v1, v2);
	return math_norm_vec3(tmp);
}

/*
	projecção de v sobre u
	r = proj(u) v
*/
void math_proj_vec3(float r[3], float v[3], float u[3])
{
	float k, dv, du;

	dv = math_dot_vec3(v, u);
	du = math_dot_vec3(u, u);
	k = dv / du;

	math_scale_vec3(r, k, u);
}

/*
	gram-schmidt
	ret = v2 - proj(v1) v2
*/
void math_gschmidt_2vec3(float ret[3], float v1[3], float v2[3])
{
	float proj[3];

	math_proj_vec3(proj, v2, v1);
	math_sub_vec3(ret, v2, proj);
}
