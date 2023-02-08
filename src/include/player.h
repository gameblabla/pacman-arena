/* $Id: player.h,v 1.3 2003/11/27 22:11:57 nsubtil Exp $ */

#ifndef _PLAYER_H
#define _PLAYER_H

#define PLAYER_STATE_STOPPED 0
#define PLAYER_STATE_MOVING 1
#define PLAYER_STATE_DEAD 2
#define PLAYER_STATE_WON 3

#define PLAYER_START_LIVES 3

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_LEFT 2
#define KEY_RIGHT 3
#define KEY_FIRE 4
#define KEY_BOMB 5
#define KEY_NUM 6

struct player
{
	/* location: local, remote */
	int location;
	/* id */
	int id;

	/* a mexer, parado, a morrer, ... */
	int state;
	/* nº de segundos que ainda dura o último "comprimido" */
	float pill_time;

	/* posição em world-space */
	float position[3];
	/* posição de arranque do jogador */
	float start_position[3];
	/* (x,y) do tile mais próximo no mapa */
/*	int tile[2];*/
	/* vector direcção (2D, referencial do mapa) */
	int direction;

	/* animação para o pacman parado */
	struct object *model_stopped;
	int frames_stopped;

	/* animação para o pacman a mexer */
	struct object *model_moving;
	int frames_moving;

	/* animação para o pacman quando morre */
	struct object *model_dying;
	int frames_dying;

	/* animação para o pacman quando ganha */
	struct object *model_won;
	int frames_won;

	/* frame actual da animação */
	float current_frame;

	/* teclas: cima, baixo, esquerda, direita */
	int keys[KEY_NUM];

	/* velocidade (tiles / segundo) */
	float speed;
	/* vidas */
	int lives;

	/* cor do pacman */
	GLfloat *color;

	/* camera deste jogador */
	struct camera *camera;
	/* viewport deste jogador */
	struct viewport *viewport;
};

#define PACMAN_YELLOW 224.0/256.0, 237.0/256.0, 49.0/256.0, 1.0
#define PACMAN_BLUE 0.0, 202.0/256.0, 252.0/256.0, 1.0

void player_reset(struct game *game);
void player_add_new(struct game *game);
void player_update(struct game *game, int player_no, float delta);
void player_kill(struct game *game, int player_no);

#endif
