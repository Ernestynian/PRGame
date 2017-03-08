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
	char alive;
	
	char moved;
} player;


extern player players[MAX_CLIENTS];


void players_init();

void player_reset(int id);

void player_moved(char id, int x, int y);

void player_spawn(int id, int x, int y);

#endif /* PLAYERS_H */

