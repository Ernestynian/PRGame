#ifndef PLAYERS_H
#define PLAYERS_H

#include "pthread.h"


typedef struct {
	float x;
	float y;
} position;

typedef struct {
	pthread_mutex_t pos_mutex_w; // mutex for write access
	pthread_mutex_t pos_mutex_c; // mutex for readers counter
	int pos_readersCount;
	position pos;
	position speed;
	int isLookingRight;
	
	pthread_mutex_t alive_mutex_w; // mutex for write access
	pthread_mutex_t alive_mutex_c; // mutex for readers counter
	int alive_readersCount;
	char alive;
	
	char moved;
} Player;


extern Player players[MAX_CLIENTS];


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
int player_isLookingRight(char id);
void player_stopMovement(int id);

void player_spawn(int id, float x, float y);
void player_kill(int id);

int player_isAlive(int id);
int player_collides(int id, int x, int y, int w, int h);

#endif /* PLAYERS_H */

