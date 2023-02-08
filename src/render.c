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
  "$Id: render.c,v 1.46 2003/11/22 17:32:10 nsubtil Exp $";

#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "linked-lists.h"
#include "object.h"
#include "m_math.h"
#include "gfx.h"
#include "game.h"
#include "map.h"
#include "player.h"
#include "screen.h"

#include "render.h"
#include "render_bomb.h"

/*
  desenha um objecto
*/
void render_object(struct object *o)
{
	int c;
	GLfloat last_color[4];
	
	memcpy(last_color, o->fc_list[0]->color, sizeof(last_color));
	glColor4fv(last_color);
	
	glBegin(GL_TRIANGLES);

	for(c = 0; c < o->num_fc; c++)
	{
		if(memcmp(last_color, o->fc_list[c]->color, sizeof(last_color)) != 0)
		{
			memcpy(last_color, o->fc_list[c]->color, sizeof(last_color));
			glColor4fv(last_color);
		}

		glNormal3fv(o->fc_list[c]->na);
		glVertex3fv(o->fc_list[c]->a);

		glNormal3fv(o->fc_list[c]->nb);
		glVertex3fv(o->fc_list[c]->b);

		glNormal3fv(o->fc_list[c]->nc);
		glVertex3fv(o->fc_list[c]->c);
	}

	glEnd();
}

/*
  desenha os polígonos com cor definida de um objecto
*/
void render_object_colored_polygons(struct object *o)
{
	int c;
	GLfloat last_color[4];

	/* tirar a primeira cor */
	for(c = 0; c < o->num_fc; c++)
		if(!o->fc_list[0]->color_tag)
		{
			memcpy(last_color, o->fc_list[0]->color, sizeof(last_color));
			break;
		}
	
	glColor4fv(last_color);
	glBegin(GL_TRIANGLES);
	
	for(c = 0; c < o->num_fc; c++)
		if(!o->fc_list[c]->color_tag)
		{
			if(memcmp(last_color, o->fc_list[c]->color, sizeof(last_color)) != 0)
			{
				memcpy(last_color, o->fc_list[c]->color, sizeof(last_color));
				glColor4fv(last_color);
			}
		
			glNormal3fv(o->fc_list[c]->na);
			glVertex3fv(o->fc_list[c]->a);
		
			glNormal3fv(o->fc_list[c]->nb);
			glVertex3fv(o->fc_list[c]->b);
			
			glNormal3fv(o->fc_list[c]->nc);
			glVertex3fv(o->fc_list[c]->c);
		}
	
	glEnd();
}

/*
  desenha polígonos sem cor
*/
void render_object_no_colored_polygons(struct object *o)
{
	int c;

	glBegin(GL_TRIANGLES);
	
	for(c = 0; c < o->num_fc; c++)
		if(o->fc_list[c]->color_tag)
		{
			glNormal3fv(o->fc_list[c]->na);
			glVertex3fv(o->fc_list[c]->a);
			
			glNormal3fv(o->fc_list[c]->nb);
			glVertex3fv(o->fc_list[c]->b);
			
			glNormal3fv(o->fc_list[c]->nc);
			glVertex3fv(o->fc_list[c]->c);
		}

	glEnd();
}

void render_object_color_tag(struct object *o, GLfloat color[4])
{
	int c;
	GLfloat last_color[4];

	if(o->fc_list[0]->color_tag)
		memcpy(last_color, color, sizeof(last_color));
	else
		memcpy(last_color, o->fc_list[0]->color, sizeof(last_color));
	glColor4fv(last_color);
	
	glBegin(GL_TRIANGLES);

	for(c = 0; c < o->num_fc; c++)
	{
		if(o->fc_list[c]->color_tag)
		{
			if(memcmp(last_color, color, sizeof(last_color)) != 0)
			{
				memcpy(last_color, color, sizeof(last_color));
				glColor4fv(last_color);
			}
		} else {
			if(memcmp(last_color, o->fc_list[c]->color, sizeof(last_color)) != 0)
			{
				memcpy(last_color, o->fc_list[c]->color, sizeof(last_color));
				glColor4fv(last_color);
			}
		}

		glNormal3fv(o->fc_list[c]->na);
		glVertex3fv(o->fc_list[c]->a);

		glNormal3fv(o->fc_list[c]->nb);
		glVertex3fv(o->fc_list[c]->b);

		glNormal3fv(o->fc_list[c]->nc);
		glVertex3fv(o->fc_list[c]->c);
	}

	glEnd();
}

/*
  desenha um objecto por display lists
*/
void render_dlist(struct object *o, GLfloat *color)
{
	if(o->dlist_color != -1)
	{
		glCallList(o->dlist_color);
		if(color)
			glColor4fv(color);

		glCallList(o->dlist_nocolor);
	} else {
		if(color)
			render_object_color_tag(o, color);
		else
			render_object(o);
	}
}

/*
  compila a display list de um objecto
*/
void render_compile_dlist(struct object *o)
{
	if(o->dlist_color != -1)
	{
		glDeleteLists(o->dlist_color, 1);
		glDeleteLists(o->dlist_nocolor, 1);
	}
	
	o->dlist_color = glGenLists(1);
	glNewList(o->dlist_color, GL_COMPILE);
	render_object_colored_polygons(o);
	glEndList();

	o->dlist_nocolor = glGenLists(1);
	glNewList(o->dlist_nocolor, GL_COMPILE);
	render_object_no_colored_polygons(o);
	glEndList();
}

/*
  inicia o desenho de uma nova frame
*/
void render_start_frame(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void render_setup(struct game *game, int player_no)
{
	struct camera *camera;
	struct viewport *vp;

	/* light0: centro do mapa, aponta para baixo, spot branco */
	GLfloat light0_ambient[] = {0.1, 0.1, 0.1, 1.0};
	GLfloat light0_diffuse[] = {0.8, 0.8, 0.8, 1.0};
	GLfloat light0_specular[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light0_position[] =
		{(GLfloat)game->map->width / 2.0, -15.0, (GLfloat)game->map->height / 2.0, 1.0};
	GLfloat light0_direction[] = 
		{0.0, 1.0, 0.0, 1.0};
	GLfloat light0_spot_cutoff = 90.0 * (game->map->width * game->map->height) / (31 * 31);
	GLfloat light0_spot_exponent = 1.0;
	/* attn: constant, linear, quadratic */
	GLfloat light0_attn[] = {1.0, 0.0, 0.0};

	/* light1: por cima do pacman, aponta para baixo, spot amarelo */
	GLfloat light1_ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light1_diffuse[] = {1.0, 0.0, 0.0, 1.0};
	GLfloat light1_specular[] = {0.0, 0.0, 0.0, 1.0};
	/* XXX! */
	GLfloat light1_position[] = 
//		{(GLfloat)players[0].position[X], -15.0, (GLfloat)players[0].position[Z], 1.0};
	{0.0, 0.0, 0.0, 1.0};
	GLfloat light1_direction[] = 
		{0.0, 1.0, 0.0, 1.0};
	GLfloat light1_spot_cutoff = 20.0;
	GLfloat light1_spot_exponent = 20.0;
	GLfloat light1_attn[] = {1.0, 0.0, 0.0};

	if(player_no == -1)
	{
		/* special case: demo mode */
		camera = game->demo_camera;
		screen_set_active_viewport(0);
		vp = screen_get_viewport(0);
	} else {
		camera = game->players[player_no].camera;
		screen_set_active_viewport(player_no);
		vp = screen_get_viewport(player_no);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0,
		  -1.0 * (GLfloat)vp->height / (GLfloat)vp->width,
		  1.0 * (GLfloat)vp->height / (GLfloat)vp->width, 1.0, 70.0);

	gluLookAt(camera->pos[X], camera->pos[Y], camera->pos[Z],
		  camera->dir[X], camera->dir[Y], camera->dir[Z],
		  camera->pos[X] - camera->up[X],
		  camera->pos[Y] - camera->up[Y],
		  camera->pos[Z] - camera->up[Z]);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_LIGHTING);

	/* light 0 */
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);

	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, light0_spot_cutoff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, light0_spot_exponent);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, light0_attn[0]);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light0_attn[1]);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, light0_attn[2]);
	
	glEnable(GL_LIGHT0);

	if(game->n_players > 0)
	{
		light1_position[X] = (GLfloat)game->players[0].position[X];
		light1_position[Y] = -15.0;
		light1_position[Z] = (GLfloat)game->players[0].position[Z];

		/* light 1 */
		glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
	
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, light1_spot_cutoff);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, light1_spot_exponent);
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, light1_attn[0]);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, light1_attn[1]);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, light1_attn[2]);
	}

	glEnable(GL_COLOR_MATERIAL);

	glDepthFunc(GL_LEQUAL);
}

/*
  calcula a geometria world-space de um quad
  perpendicular à camera com size de lado
*/
static GLfloat plane_vertexes[4][3];
static GLuint plane_dlist = -1;
void render_setup_plane_geometry(struct camera *camera, float size)
{
	float tmp[3], cam[3], up[3];

	math_sub_vec3(cam, camera->dir, camera->pos);
	math_sub_vec3(up, camera->up, camera->pos);
	math_rotate_vec3(tmp, cam, up, 90.0);
	
	math_add_vec3(plane_vertexes[0], tmp, up);
	math_scale_vec3(plane_vertexes[0], -1.0, plane_vertexes[0]);
	math_len_vec3(plane_vertexes[0], plane_vertexes[0], size / 2.0);

	math_sub_vec3(plane_vertexes[1], tmp, up);
	math_len_vec3(plane_vertexes[1], plane_vertexes[1], size / 2.0);
	
	math_add_vec3(plane_vertexes[2], tmp, up);
	math_len_vec3(plane_vertexes[2], plane_vertexes[2], size / 2.0);
	
	math_sub_vec3(plane_vertexes[3], up, tmp);
	math_len_vec3(plane_vertexes[3], plane_vertexes[3], size / 2.0);

	if(plane_dlist == -1)
		plane_dlist = glGenLists(1);

	glNewList(plane_dlist, GL_COMPILE);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3fv(plane_vertexes[3]);
	glTexCoord2f(1.0, 1.0);
	glVertex3fv(plane_vertexes[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3fv(plane_vertexes[1]);
	glTexCoord2f(0.0, 0.0);
	glVertex3fv(plane_vertexes[0]);
	glEnd();

	glEndList();
}

/*
  desenha um quad perpendicular à camera
  com origem em (x, y, z) e size de lado
*/
void render_draw_plane(GLfloat x, GLfloat y, GLfloat z, GLfloat alpha)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, z);

	glBlendFunc(GL_ONE, GL_ONE);
	glColor4f(alpha, alpha, alpha, 1.0);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glCallList(plane_dlist);
}

void render_draw_colored_plane(GLfloat x, GLfloat y, GLfloat z, GLfloat color[3], GLfloat alpha)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, z);
	
	glBlendFunc(GL_ONE, GL_ONE);
	glColor4f(color[R] - (1.0 - alpha),
		  color[G] - (1.0 - alpha),
		  color[B] - (1.0 - alpha),
		  1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glCallList(plane_dlist);
}

/*
  termina o desenho de uma frame
*/
void render_finish_frame(void)
{
	glFlush();
	SDL_GL_SwapBuffers();
}

/*
  desenha os eixos
*/
void render_draw_axes(float length)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_LIGHTING);
	glDisable(GL_LINE_SMOOTH);

	glBegin(GL_LINES);

	/* x */
	glColor3f(0.25, 0.0, 0.0);
	glVertex3f(-length, 0.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(length, 0.0, 0.0);
	
	/* y */
	glColor3f(0.0, 0.25, 0.0);
	glVertex3f(0.0, -length, 0.0);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, length, 0.0);
	
	/* z */
	glColor3f(0.0, 0.0, 0.25);
	glVertex3f(0.0, 0.0, -length);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, length);
	
	glEnd();
}

/*
  posiciona a câmera em world-space
*/
void render_move_camera(struct camera *camera, float x, float y, float z)
{
	float cam[3], up[3];
	
	math_sub_vec3(cam, camera->dir, camera->pos);
	math_sub_vec3(up, camera->up, camera->pos);
	
	camera->pos[X] = x;
	camera->pos[Y] = y;
	camera->pos[Z] = z;
	
	math_add_vec3(camera->dir, camera->pos, cam);
	math_add_vec3(camera->up, camera->pos, up);
}

/*
  translacção da camera em world-space
*/
void render_translate_camera(struct camera *camera, float x, float y, float z)
{
	camera->pos[X] += x;
	camera->pos[Y] += y;
	camera->pos[Z] += z;

	camera->up[X] += x;
	camera->up[Y] += y;
	camera->up[Z] += z;

	camera->dir[X] += x;
	camera->dir[Y] += y;
	camera->dir[Z] += z;
}

/*
  aponta a câmera para um ponto
*/
void render_point_camera(struct camera *camera, float x, float y, float z)
{
	float cam[3], up[3];

	cam[X] = x - camera->pos[X];
	cam[Y] = y - camera->pos[Y];
	cam[Z] = z - camera->pos[Z];

	up[X] = 0.0;
	up[Y] = 1.0;
	up[Z] = 0.0;

	math_gschmidt_2vec3(up, cam, up);
	math_len_vec3(cam, cam, 1.0);
	math_len_vec3(up, up, 1.0);

	math_add_vec3(camera->dir, camera->pos, cam);
	math_add_vec3(camera->up, camera->pos, up);
}

/*
  camera em (0, 0, 0), apontada para (0, 0, 1), up (0, -1, 0)
*/
void render_reset_camera(struct camera *camera)
{
	camera->pos[X] = 0.0;
	camera->pos[Y] = 0.0;
	camera->pos[Z] = 0.0;

	render_reset_camera_dir(camera);
}

/*
  aponta a camera do ponto actual na direcção de ez, up (0, -1, 0)
*/
void render_reset_camera_dir(struct camera *camera)
{
	camera->dir[X] = camera->pos[X];
	camera->dir[Y] = camera->pos[Y];
	camera->dir[Z] = camera->pos[Z] + 1.0;
	
	camera->up[X] = camera->pos[X];
	camera->up[Y] = camera->pos[Y] + 1.0;
	camera->up[Z] = camera->pos[Z];
}

/*
  translacção segundo o sistema de eixos local da camera
  (constituído por cam, up e vector resultante da rotação de cam 90
  graus em sentido directo em torno de up)
*/
void render_advance_camera(struct camera *camera, float x, float y, float z)
{
	float r[3], cam[3], up[3];
	float d[3], tmp[3];
	
	/* cam = cam_dir - cam_pos */
	math_sub_vec3(cam, camera->dir, camera->pos);
	/* up = cam_up - cam_pos */
	math_sub_vec3(up, camera->up, camera->pos);
	
	/* x: seguir cam rodado 90 graus em torno de up */
	math_rotate_vec3(r, cam, up, 90.0);
	math_scale_vec3(d, x, r);

	/* y: seguir up */
	math_scale_vec3(tmp, y, up);
	math_add_vec3(d, d, tmp);

	/* z: seguir cam */
	math_scale_vec3(tmp, z, cam);
	math_add_vec3(d, d, tmp);

	math_add_vec3(camera->pos, camera->pos, d);
	math_add_vec3(camera->dir, camera->dir, d);
	math_add_vec3(camera->up, camera->up, d);
}

/*
  rotação da camera no "seu" plano xOz ("esquerda/direita")
  (cam roda em torno de up)
*/
void render_camera_yaw(struct camera *camera, float angle)
{
	float cam[3], up[3];
	
	math_sub_vec3(cam, camera->dir, camera->pos);
	math_sub_vec3(up, camera->up, camera->pos);

	math_rotate_vec3(cam, cam, up, angle);

	math_add_vec3(camera->dir, camera->pos, cam);
}

/*
  rotação da camera no "seu" plano xOy
  (up roda em torno de cam)
*/
void render_camera_roll(struct camera *camera, float angle)
{
	float cam[3], up[3];
	
	math_sub_vec3(cam, camera->dir, camera->pos);
	math_sub_vec3(up, camera->up, camera->pos);
	
	math_rotate_vec3(up, up, cam, angle);

	math_add_vec3(camera->up, camera->pos, up);
}

/*
  rotação da camera no "seu" plano yOz ("cima/baixo")
  (cam e up rodam em torno de um vector perpendicular aos dois)
*/
void render_camera_pitch(struct camera *camera, float angle)
{
	float cam[3], up[3], axis[3];
	
	math_sub_vec3(cam, camera->dir, camera->pos);
	math_sub_vec3(up, camera->up, camera->pos);

	/* vector perpendicular: rodar cam 90 graus em torno de up */
	math_rotate_vec3(axis, cam, up, 90.0);

	/* rodar cam em torno de axis */
	math_rotate_vec3(cam, cam, axis, angle);

	/* rodar up em torno de axis */
	math_rotate_vec3(up, up, axis, angle);
	
	math_add_vec3(camera->dir, camera->pos, cam);
	math_add_vec3(camera->up, camera->pos, up);
}

/*
  rotação da camera em torno de (0.0, 1.0, 0.0)
*/
void render_camera_turn(struct camera *camera, float angle)
{
	float cam[3], up[3], axis[3] = {0.0, 1.0, 0.0};

	math_sub_vec3(cam, camera->dir, camera->pos);
	math_sub_vec3(up, camera->up, camera->pos);
	
	math_rotate_vec3(cam, cam, axis, angle);
	math_rotate_vec3(up, up, axis, angle);
	
	math_add_vec3(camera->dir, camera->pos, cam);
	math_add_vec3(camera->up, camera->pos, up);
}

/*
  configura o opengl para desenhar a 2d
*/
void render_setup_2d(struct viewport *vp)
{
	glViewport(0, 0, vp->width, vp->height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/* XXX - glOrtho might need physical screen dimensions */
	glOrtho(0, vp->width, vp->height, 0, -99999, 99999);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
//	glDisable(GL_ALPHA_TEST);
}

void render_draw_scaled_image(char *image, int ul_x, int ul_y, int lr_x, int lr_y)
{
	struct image_rgba32 *img;

	img = gfx_get(image);
	if(img->id == -1)
		gfx_upload_texture(image);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, img->id);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glColor4f(1.0, 1.0, 1.0, 1.0);

	glBegin(GL_QUADS);
	
	glTexCoord2f(0.0, 0.0);
	glVertex2i(ul_x, ul_y);
	
	glTexCoord2f(0.0, 1.0);
	glVertex2i(ul_x, lr_y);
	
	glTexCoord2f(1.0, 1.0);
	glVertex2i(lr_x, lr_y);
	
	glTexCoord2f(1.0, 0.0);
	glVertex2i(lr_x, ul_y);
	
	glEnd();
}

void render_setup_model_direction(int direction)
{
	switch(direction)
	{
	case DIRECTION_RIGHT:
		/* virado para ex */
		glRotatef(90.0, 0.0, 1.0, 0.0);
		break;

	case DIRECTION_LEFT:
		/* virado para -ex */
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		break;

	case DIRECTION_DOWN:
		/* virado para -ez */
		glRotatef(180.0, 0.0, 1.0, 0.0);
		break;
	}

	/* compensar sistema de eixos */
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glRotatef(180.0, 0.0, 0.0, 1.0);
}
