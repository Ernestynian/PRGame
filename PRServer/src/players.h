#ifndef PLAYERS_H
#define PLAYERS_H

#include "pthread.h"

typedef struct {
	float x;
	float y;
} position;

typedef struct {
	pthread_mutex_t mutex;
	
	position pos;
	position speed;
	char alive;
	
	char moved;
} player;


extern player players[MAX_CLIENTS];


void players_init();

void player_reset(int id);

void player_moved(char id, float x, float y, float vx, float vy);

void player_spawn(int id, float x, float y);
void player_kill(int id);

#endif /* PLAYERS_H */

