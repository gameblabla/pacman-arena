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

/* $Id: particle.h,v 1.2 2003/11/22 17:32:10 nsubtil Exp $ */

#ifndef _PARTICLE_H
#define _PARTICLE_H

#define PARTICLE_STATE_DEAD 0
#define PARTICLE_STATE_ACTIVE 1

struct particle
{
	/* particle state (dead, active, ...) */
	int state;
	/* time */
	float time;
	/* 3D world-space position */
	float position[3];
	/* constant direction vector */
	float direction[3];
	/* particle color */
	float color[3];
	/* speed */
	float speed;
	/* gravity */
	float gravity;
};

struct particle_src
{
	/* number of particle structures */
	int n_particles;
	/* particles */
	struct particle *particles;
	
	/* life of each particle */
	float particle_life;
	/* fade time of each particle */
	float particle_fade;
	/* particle generation rate */
	float particle_rate;
	/* last generation time */
	float last_particle_time;
	/* particle size */
	float particle_size;

	/* fountain position */
	float position[3];
	/* fountain speed vector */
	float src_speed[3];
	/* fountain direction */
	float direction[3];
	/* maximum direction spread */
	float spread;
	/* speed */
	float speed;
	/* maximum speed variation */
	float speed_spread;
	/* fountain gravity */
	float gravity;
	/* particle colors */
	float color[3];

	/* light source for particle */
	struct light *light_src;
};

struct particle_src *particle_new_src(float life, float fade, float rate, float size,
				      float position[3], float direction[3], float src_speed[3],
				      float spread, float speed, float speed_spread,
				      float gravity, float color[4]);
int particle_add(struct particle_src *src);
void particle_src_update(struct game *game, struct particle_src *src, float delta);
void particle_update(struct game *game, struct particle_src *src, int particle_no, float delta);
void particle_free_src(struct particle_src *src);
int particle_src_all_dead(struct particle_src *src);
void particle_src_explode(struct particle_src *src, int num_particles_explosion, float max_speed);
#endif
