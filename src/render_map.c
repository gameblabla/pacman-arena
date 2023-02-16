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
  "$Id: render_map.c,v 1.4 2003/11/30 17:43:55 nsubtil Exp $";

#include "common.h" // Default headers

#ifdef PSP2
#include "vita_GLU.h"
#else
#include <GL/glu.h>
#endif

#include "object.h"
#include "game.h"
#include "player.h"
#include "render.h"
#include "gfx.h"
#include "m_math.h"
#include "shot.h"

#include "map.h"


static struct object *wall[12];
#define WALL_HORIZONTAL 0
#define WALL_LL			1
#define WALL_LR			2
#define WALL_UL			3
#define WALL_UR			4
#define WALL_VERTICAL	5

static struct image_rgba32 *food = NULL;
static struct image_rgba32 *rocket_lightmap = NULL;
static GLUquadricObj *sphere = NULL;
static struct image_rgba32 *missile_texture = NULL;

/* It's better to preload the graphics than to have to run a condition that runs in a loop each time...
 * We want to avoid a potential CPU bottleneck at least.
*/
void load_map_assets()
{
	int nframes;
	wall[WALL_HORIZONTAL] = object_read_file("gfx/wall-horizontal.3d", &nframes);
	wall[WALL_VERTICAL] = object_read_file("gfx/wall-vertical.3d", &nframes);
	wall[WALL_LL] = object_read_file("gfx/wall-ll.3d", &nframes);
	wall[WALL_LR] = object_read_file("gfx/wall-lr.3d", &nframes);
	wall[WALL_UL] = object_read_file("gfx/wall-ul.3d", &nframes);
	wall[WALL_UR] = object_read_file("gfx/wall-ur.3d", &nframes);
	
	food = gfx_get("gfx/dot-yellow.tga");
	gfx_alpha_from_intensity("gfx/dot-yellow.tga");
	gfx_upload_texture("gfx/dot-yellow.tga");
	
	rocket_lightmap = gfx_get("gfx/rocket-highlight.tga");
	gfx_alpha_from_intensity("gfx/rocket-highlight.tga");
	gfx_upload_texture("gfx/rocket-highlight.tga");
	
	sphere = gluNewQuadric();
	gluQuadricNormals(sphere, GLU_FLAT);
	gluQuadricTexture(sphere, GLU_FALSE);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricDrawStyle(sphere, GLU_FILL);
	
	missile_texture = gfx_get("gfx/rocket-highlight.tga");
	gfx_upload_texture("gfx/rocket-highlight.tga");
}


/*
  prepara-se para desenhar um tile do mapa
*/
void map_setup_wall(struct game *game, int x, int y)
{
	struct map *map;

	map = game->map;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef((GLfloat)x + 0.5, -0.5, (GLfloat)y + 0.5);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
//	glDisable(GL_TEXTURE_2D);
//	glDisable(GL_BLEND);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

#if 0
	/* compute lightmaps */
	for(c = 0; c < game->n_shots; c++)
	{
		if(game->shots[c].type == SHOT_TYPE_ROCKET && game->shots[c].state == SHOT_STATE_ACTIVE)
		{
			int rx, ry;
			
			rx = (int)game->shots[c].position[X];
			ry = (int)game->shots[c].position[Z];
			
			switch(MAP(game->map, x, y).wall)
			{
			case MAP_WALL_HORIZONTAL:
				if(rx == x && (ry == y - 1 || ry == y + 1))
				{
					GLfloat params[4];

					/* XXX - multitexturing! */
					glActiveTexture(GL_TEXTURE0_ARB);
					glDisable(GL_TEXTURE_2D);
					
					glActiveTexture(GL_TEXTURE1_ARB);
					glBindTexture(GL_TEXTURE_2D, rocket_lightmap->id);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glEnable(GL_TEXTURE_2D);
					
					map_lightmap_param_rocket(params, game, x, y, c);

					params[X] = 1.0;
					params[Y] = 1.0;
					params[Z] = 1.0;
					params[W] = 1.0;
					glTexGenfv(GL_S, GL_OBJECT_PLANE, params);
					glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
					glEnable(GL_TEXTURE_GEN_S);
					
					params[X] = 1.0;
					params[Y] = 1.0;
					params[Z] = 1.0;
					params[W] = 1.0;
					glTexGenfv(GL_T, GL_OBJECT_PLANE, params);
					glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
					glEnable(GL_TEXTURE_GEN_T);
					
					glBlendFunc(GL_ONE, GL_ONE);
					glEnable(GL_BLEND);
					
					return;
				}
				
				break;
			}
		}
	}
#endif
}

/*
  computes lightmap parameters for wall at x, y and rocket n
*/
void map_lightmap_param_rocket(GLfloat params[4], struct game *game, int x, int y, int r)
{
	struct map *map;
	struct shot *rocket;
	
	map = game->map;
	rocket = &game->shots[r];
	
	/* xxx - do something nice */
	params[0] = 1.0;
	params[1] = 1.0;
	params[2] = 0.0;
	params[3] = 1.0;
}

/*
  desenha uma parede vertical em x, y
*/
void map_draw_vertical_wall(struct game *game, int x, int y)
{
	map_setup_wall(game, x, y);
	render_dlist(wall[WALL_VERTICAL], NULL);
}

/*
  desenha uma parede horizontal em (x, y)
*/
void map_draw_horizontal_wall(struct game *game, int x, int y)
{
	map_setup_wall(game, x, y);
	render_dlist(wall[WALL_HORIZONTAL], NULL);
}

/*
  canto inferior esquerdo
*/
void map_draw_ll_wall(struct game *game, int x, int y)
{
	map_setup_wall(game, x, y);
	render_dlist(wall[WALL_LL], NULL);
}

/*
  canto inferior direito
*/
void map_draw_lr_wall(struct game *game, int x, int y)
{
	map_setup_wall(game, x, y);
	render_dlist(wall[WALL_LR], NULL);
}

/*
  canto superior direito
*/
void map_draw_ur_wall(struct game *game, int x, int y)
{
	map_setup_wall(game, x, y);
	render_dlist(wall[WALL_UR], NULL);
}

/*
  canto superior esquerdo
*/
void map_draw_ul_wall(struct game *game, int x, int y)
{
	map_setup_wall(game, x, y);
	render_dlist(wall[WALL_UL], NULL);
}

/*
  desenha comida do pacman em (x,y) no mapa
*/
void map_draw_pacman_food(struct map *map, int x, int y)
{	
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
//	glBlendFunc(GL_ONE, GL_ONE);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, food->id);

	glDepthMask(GL_FALSE);

	switch(MAP_FOOD(map, x, y).type)
	{
	case FOOD_TYPE_NORMAL:
		if(MAP_FOOD(map, x, y).status == FOOD_STATUS_ACTIVE)
			render_draw_plane((GLfloat)x + 0.5, -0.5, (GLfloat)y + 0.5, 1.0);

		break;

	case FOOD_TYPE_RESPAWN:
		if(MAP_FOOD(map, x, y).status == FOOD_STATUS_EATEN)
		{
			if(MAP_FOOD(map, x, y).time <= MAP_FOOD(map, x, y).fade_in)
				render_draw_plane((GLfloat)x + 0.5, -0.5, (GLfloat)y + 0.5,
						  1.0 - (MAP_FOOD(map, x, y).time /
							 MAP_FOOD(map, x, y).fade_in));
		} else
			/* active */
			render_draw_plane((GLfloat)x + 0.5, -0.5, (GLfloat)y + 0.5, 1.0);
		
		break;
		
	case FOOD_TYPE_INTERMITTENT:
		if(MAP_FOOD(map, x, y).status == FOOD_STATUS_ACTIVE)
		{
			if(MAP_FOOD(map, x, y).time <= MAP_FOOD(map, x, y).fade_out)
				render_draw_plane((GLfloat)x + 0.5, -0.5, (GLfloat)y + 0.5,
						  (MAP_FOOD(map, x, y).time /
						   MAP_FOOD(map, x, y).fade_out));
			else
				render_draw_plane((GLfloat)x + 0.5, -0.5, (GLfloat)y + 0.5, 1.0);
		} else {
			if(MAP_FOOD(map, x, y).time <= MAP_FOOD(map, x, y).fade_in)
				render_draw_plane((GLfloat)x + 0.5, -0.5, (GLfloat)y + 0.5,
						  1.0 - (MAP_FOOD(map, x, y).time /
							 MAP_FOOD(map, x, y).fade_in));
		}
		
		break;
	}
			
	glDepthMask(GL_TRUE);
}

void map_draw_pacman_pill(struct map *map, int x, int y)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* XXX - isto a mexer fica mais giro */
//	glTranslatef((GLfloat)x + 0.5, pill_height, (GLfloat)y + 0.5);
//	glTranslatef((GLfloat)x + 0.5, MAP(map, x, y).c_data.pill.height, (GLfloat)y + 0.5);
	glTranslatef((GLfloat)x + 0.5,
		     -0.5 + 0.25 * sin(PILL_BOB_FREQUENCY * (float)SDL_GetTicks() / 1000.0 +
				       MAP(map, x, y).c_data.pill.phase),
		     (GLfloat)y + 0.5);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glColor4f(0.91, 0.88, 0.28, 1.0);
	gluSphere(sphere, 0.2, 15, 15);
}

void map_draw_wall(struct game *game, int x, int y)
{
	struct map *map;
	
	map = game->map;
	
	switch(MAP(map, x, y).wall)
	{
	case MAP_WALL_HORIZONTAL:
		map_draw_horizontal_wall(game, x, y);
		break;
		
	case MAP_WALL_VERTICAL:
		map_draw_vertical_wall(game, x, y);
		break;
		
	case MAP_WALL_LL:
		map_draw_ll_wall(game, x, y);
		break;
		
	case MAP_WALL_UL:
		map_draw_ul_wall(game, x, y);
		break;
		
	case MAP_WALL_LR:
		map_draw_lr_wall(game, x, y);
		break;
		
	case MAP_WALL_UR:
		map_draw_ur_wall(game, x, y);
		break;

	default:
		break;
	}
}

void map_render_opaque_objects(struct game *game)
{
	int x, y;
	struct map *map;
	
	map = game->map;

	/* pass 1: untextured geometry */

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	
	for(x = 0; x < map->width; x++)
		for(y = 0; y < map->height; y++)
		{
			map_draw_wall(game, x, y);

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);

			switch(MAP(map, x, y).content)
			{
			case MAP_CONTENT_PILL:
				if(MAP(map, x, y).c_data.pill.status == PILL_STATUS_ACTIVE)
					map_draw_pacman_pill(map, x, y);
				break;
			}
		}

#ifdef UGLY_LIGHTMAPS
	/* pass 2: additive lightmaps */
	for(c = 0; c < game->n_shots; c++)
		switch(game->shots[c].type)
		{
		case SHOT_TYPE_ROCKET:
			if(game->shots[c].state != SHOT_STATE_ACTIVE)
				continue;

			map_draw_rocket_lightmap(game, c);
			break;
		}
#endif
}

void map_draw_rocket_lightmap(struct game *game, int c)
{
	struct shot *rocket;
	int x, y;
	
	/* object plane coefficients (s, t, r, q) */
	GLfloat params[4];
	
	rocket = &game->shots[c];

	/* set up texturing */
	glBindTexture(GL_TEXTURE_2D, missile_texture->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);
	
	/* texture coordinate generation */
	switch(rocket->direction)
	{
	case DIRECTION_LEFT:
		params[X] = -1.0;
		params[Y] = 0.0;
		params[Z] = 1.0;
		params[W] = 1.0;
		break;
		
	case DIRECTION_RIGHT:
		params[X] = 1.0;
		params[Y] = 0.0;
		params[Z] = 1.0;
		params[W] = 1.0;
		break;

	case DIRECTION_UP:
		params[X] = 0.0;
		params[Y] = 0.0;
		params[Z] = 1.0;
		params[W] = 1.0;
		break;

	case DIRECTION_DOWN:
		params[X] = 0.0;
		params[Y] = 0.0;
		params[Z] = -1.0;
		params[W] = 1.0;
		break;
	}

	glTexGenfv(GL_S, GL_OBJECT_PLANE, params);	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	
	params[X] = 0.0;
	params[Y] = 1.0;
	params[Z] = 0.0;
	params[W] = 1.0;
	glTexGenfv(GL_T, GL_OBJECT_PLANE, params);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_T);
	
	/* additive alpha blending */
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

	glColor4f(1.0, 1.0, 1.0, 1.0);

	/* draw map squares */
	x = (int)rocket->position[X];
	y = (int)rocket->position[Z];

	switch(rocket->direction)
	{
	case DIRECTION_LEFT:
		/* up right square */

		/* glPolygonOffset and glDepthFunc(GL_LESS) might be a better idea
		   instead of messing with the projection matrix */
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glTranslatef(0.0, 0.0, -0.01);

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(-(rocket->position[X] - (float)x) + 1.0, -0.5, 0.3);

#if 0
		switch(MAP(game->map, x + 1, y + 1).wall)
		{
		case MAP_WALL_LL:
			/* fix texturing for LL corners */
			printf("fixing LL corner for UR\n");
			params[X] = -1.0;
			params[Y] = 0.0;
			params[Z] = 1.0;
			params[W] = 1.0;
			break;

		default:
			params[X] = -1.0;
			params[Y] = 0.0;
			params[Z] = 1.0;
			params[W] = 1.0;
			break;
		}

		glTexGenfv(GL_S, GL_OBJECT_PLANE, params);
#endif
		map_draw_wall(game, x + 1, y + 1);

		/* up square */
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(-(rocket->position[X] - (float)x), -0.5, 0.3);

#if 0		
		switch(MAP(game->map, x, y + 1).wall)
		{
		case MAP_WALL_LL:
			/* fix texturing for LL corners */
			printf("fixing LL corner for UP\n");
			params[X] = -1.0;
			params[Y] = 0.0;
			params[Z] = 1.0;
			params[W] = 1.0;
			break;

		default:
			params[X] = -1.0;
			params[Y] = 0.0;
			params[Z] = 1.0;
			params[W] = 1.0;
			break;
		}

		glTexGenfv(GL_S, GL_OBJECT_PLANE, params);
#endif
		map_draw_wall(game, x, y + 1);


		/* down right square */
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.01);
		
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(-(rocket->position[X] - (float)x), -0.5, 0.3);

		map_draw_wall(game, x + 1, y - 1);
		
		/* down square */
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(-(rocket->position[X] - (float)x) - 1.0, -0.5, 0.3);

		map_draw_wall(game, x, y - 1);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		
		break;

	case DIRECTION_RIGHT:
		/* up left square */
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glTranslatef(0.0, 0.0, -0.01);
		
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(-(rocket->position[X] - (float)x), -0.5, 0.0);
		
		map_draw_wall(game, x - 1, y + 1);

		/* up square */
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(-(rocket->position[X] - (float)x) + 1.0, -0.5, 0.0);

		map_draw_wall(game, x, y + 1);


		/* down left square */
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.0, 0.0, 0.01);
		
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(-(rocket->position[X] - (float)x) - 1.0, -0.5, 0.0);
		
		map_draw_wall(game, x - 1, y - 1);
		
		/* down square */
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(-(rocket->position[X] - (float)x), -0.5, 0.0);
		
		map_draw_wall(game, x, y - 1);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		
		break;

	case DIRECTION_UP:
		/* left down square */
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glTranslatef(0.01, 0.0, 0.0);
		
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.0, -0.5, -(rocket->position[Z] - (float)y));
		
		map_draw_wall(game, x - 1, y - 1);
		
		/* left square */
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.0, -0.5, -(rocket->position[Z] - (float)y) + 1.0);
		
		map_draw_wall(game, x - 1, y);

		/* right down square */
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-0.01, 0.0, 0.0);
		
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.0, -0.5, -(rocket->position[Z] - (float)y));

		map_draw_wall(game, x + 1, y - 1);
		
		/* right square */
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.0, -0.5, -(rocket->position[Z] - (float)y) + 1.0);
		
		map_draw_wall(game, x + 1, y);
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		break;

	case DIRECTION_DOWN:
		/* left up square */
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glTranslatef(0.01, 0.0, 0.0);
		
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.0, -0.5, -(rocket->position[Z] - (float)y) + 1.0);
		
		map_draw_wall(game, x - 1, y + 1);
		
		/* left square */
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.0, -0.5, -(rocket->position[Z] - (float)y));
		
		map_draw_wall(game, x - 1, y);
		
		/* right up square */
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-0.01, 0.0, 0.0);
		
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.0, -0.5, -(rocket->position[Z] - (float)y) + 1.0);
		
		map_draw_wall(game, x + 1, y + 1);
		
		/* right square */
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(0.0, -0.5, -(rocket->position[Z] - (float)y));
		
		map_draw_wall(game, x + 1, y);
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		break;
	}
	
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}
	
void map_render_translucent_objects(struct game *game, int player_no)
{
	int x, y;
	struct map *map;
	struct camera *cam;

	if(player_no == -1)
		cam = game->demo_camera;
	else
		cam = game->players[player_no].camera;
		
	render_setup_plane_geometry(cam, 1.0);

	map = game->map;
	for(x = 0; x < map->width; x++)
		for(y = 0; y < map->height; y++)
			switch(MAP(map, x, y).content)
			{
			case MAP_CONTENT_FOOD:
				/* comidinha do pacman */
				map_draw_pacman_food(map, x, y);
				break;
			}
	
}
