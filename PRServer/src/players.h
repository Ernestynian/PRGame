#ifndef PLAYERS_H
#define PLAYERS_H

#include "pthread.h"

#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 38

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

/**
 * 
 * @param x - new position and optionally return previous position
 * @param y - new position and optionally return previous position
 * @return true if the move was accepted
 */
int player_moved(char id, float x, float y, float vx, float vy);
void player_getPos(char id, float* x, float* y);

void player_spawn(int id, float x, float y);
void player_kill(int id);

#endif /* PLAYERS_H */

