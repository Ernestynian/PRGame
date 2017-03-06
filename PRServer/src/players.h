#ifndef PLAYERS_H
#define PLAYERS_H

#include "pthread.h"

typedef struct {
	int x;
	int y;
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

void player_moved(int id, char* data);

void player_spawn(int id, int x, int y);

#endif /* PLAYERS_H */

