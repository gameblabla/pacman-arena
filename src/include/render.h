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

/* $Id: render.h,v 1.2 2003/11/22 17:32:10 nsubtil Exp $ */

#ifndef _RENDER_H
#define _RENDER_H

void render_object(struct object *o);
void render_object_colored_polygons(struct object *o);
void render_object_no_colored_polygons(struct object *o);
void render_object_color_tag(struct object *o, GLfloat color[4]);
void render_dlist(struct object *o, GLfloat *color);
void render_compile_dlist(struct object *o);
void render_start_frame(void);
void render_setup(struct game *game, int player_no);
void render_setup_plane_geometry(struct camera *camera, float size);
void render_draw_plane(GLfloat x, GLfloat y, GLfloat z, GLfloat alpha);
void render_draw_colored_plane(GLfloat x, GLfloat y, GLfloat z, GLfloat color[3], GLfloat alpha);
void render_finish_frame(void);
void render_draw_axes(float length);
void render_move_camera(struct camera *camera, float x, float y, float z);
void render_translate_camera(struct camera *camera, float x, float y, float z);
void render_point_camera(struct camera *camera, float x, float y, float z);
void render_reset_camera(struct camera *camera);
void render_reset_camera_dir(struct camera *camera);
void render_advance_camera(struct camera *camera, float x, float y, float z);
void render_camera_yaw(struct camera *camera, float angle);
void render_camera_roll(struct camera *camera, float angle);
void render_camera_pitch(struct camera *camera, float angle);
void render_camera_turn(struct camera *camera, float angle);
void render_setup_2d(struct viewport *vp);
void render_draw_scaled_image(char *image, int ul_x, int ul_y, int lr_x, int lr_y);
void render_setup_model_direction(int direction);

#endif
