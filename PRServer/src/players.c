#include <stdio.h>
#include <pthread.h>

#include "../../Common/networkInterface.h"

#include "players.h"


Player players[MAX_CLIENTS];


void players_init() {
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		pthread_mutex_init(&players[i].pos_mutex_w, NULL);
		pthread_mutex_init(&players[i].pos_mutex_c, NULL);
		players[i].pos_readersCount = 0;
		pthread_mutex_init(&players[i].alive_mutex_w, NULL);
		pthread_mutex_init(&players[i].alive_mutex_c, NULL);
		players[i].alive_readersCount = 0;
	}
}


void player_reset(int id) {
	pthread_mutex_lock(&players[id].pos_mutex_c);
	players[id].pos_readersCount = 0;
	pthread_mutex_unlock(&players[id].pos_mutex_c);
	
	pthread_mutex_lock(&players[id].alive_mutex_c);
	players[id].alive_readersCount = 0;
	pthread_mutex_unlock(&players[id].alive_mutex_c);
	
	pthread_mutex_lock(&players[id].pos_mutex_w);
	players[id].pos.x   = 0;
	players[id].pos.y   = 0;
	players[id].speed.x = 0;
	players[id].speed.y = 0;
	players[id].moved   = 0;
	pthread_mutex_unlock(&players[id].pos_mutex_w);
	
	pthread_mutex_lock(&players[id].alive_mutex_w);
	players[id].alive   = 0;
	pthread_mutex_unlock(&players[id].alive_mutex_w);
}


int player_moved(char id, float x, float y, float vx, float vy) {
	pthread_mutex_lock(&players[id].pos_mutex_w);
	players[id].pos.x   = x;
	players[id].pos.y   = y;
	players[id].speed.x = vx;
	players[id].speed.y = vy;
	if (vx != 0)
		players[id].isLookingRight = vx > 0;
	players[id].moved   = 1;
	pthread_mutex_unlock(&players[id].pos_mutex_w);
	
	return 1;
}


void player_getPos(char id, float* x, float* y) {
	pthread_mutex_lock(&players[id].pos_mutex_c);
	players[id].pos_readersCount++;
	if (players[id].pos_readersCount == 1)
		pthread_mutex_lock(&players[id].pos_mutex_w);
	pthread_mutex_unlock(&players[id].pos_mutex_c);
	
	*x = players[id].pos.x;
	*y = players[id].pos.y;
	
	pthread_mutex_lock(&players[id].pos_mutex_c);
	players[id].pos_readersCount--;
	if (players[id].pos_readersCount == 0)
		pthread_mutex_unlock(&players[id].pos_mutex_w);
	pthread_mutex_unlock(&players[id].pos_mutex_c);
}


int player_isLookingRight(char id) {
	pthread_mutex_lock(&players[id].pos_mutex_c);
	players[id].pos_readersCount++;
	if (players[id].pos_readersCount == 1)
		pthread_mutex_lock(&players[id].pos_mutex_w);
	pthread_mutex_unlock(&players[id].pos_mutex_c);
	
	int isLookingRight = players[id].isLookingRight;
	
	pthread_mutex_lock(&players[id].pos_mutex_c);
	players[id].pos_readersCount--;
	if (players[id].pos_readersCount == 0)
		pthread_mutex_unlock(&players[id].pos_mutex_w);
	pthread_mutex_unlock(&players[id].pos_mutex_c);
	
	return isLookingRight;
}


void player_stopMovement(int id) {
	pthread_mutex_lock(&players[id].pos_mutex_w);
	players[id].speed.x = 0;
	players[id].speed.y = 0;
	pthread_mutex_unlock(&players[id].pos_mutex_w);
}


void player_spawn(int id, float x, float y) {
	pthread_mutex_lock(&players[id].pos_mutex_w);
	players[id].pos.x = x;
	players[id].pos.y = y;
	pthread_mutex_unlock(&players[id].pos_mutex_w);
	
	pthread_mutex_lock(&players[id].alive_mutex_w);
	players[id].alive = 1;
	pthread_mutex_unlock(&players[id].alive_mutex_w);
}


void player_kill(int id) {
	pthread_mutex_lock(&players[id].alive_mutex_w);
	players[id].alive = 0;
	pthread_mutex_unlock(&players[id].alive_mutex_w);
}


int player_isAlive(int id) {
	pthread_mutex_lock(&players[id].alive_mutex_c);
	players[id].alive_readersCount++;
	if (players[id].alive_readersCount == 1)
		pthread_mutex_lock(&players[id].alive_mutex_w);
	pthread_mutex_unlock(&players[id].alive_mutex_c);
	
	int isAlive = players[id].alive;
	
	pthread_mutex_lock(&players[id].alive_mutex_c);
	players[id].alive_readersCount--;
	if (players[id].alive_readersCount == 0)
		pthread_mutex_unlock(&players[id].alive_mutex_w);
	pthread_mutex_unlock(&players[id].alive_mutex_c);
	
	return isAlive;
}


int player_collides(int id, int x, int y, int w, int h) {
	pthread_mutex_lock(&players[id].pos_mutex_c);
	players[id].pos_readersCount++;
	if (players[id].pos_readersCount == 1)
		pthread_mutex_lock(&players[id].pos_mutex_w);
	pthread_mutex_unlock(&players[id].pos_mutex_c);
	
	int px = players[id].pos.x;
	int py = players[id].pos.y;
	
	pthread_mutex_lock(&players[id].pos_mutex_c);
	players[id].pos_readersCount--;
	if (players[id].pos_readersCount == 0)
		pthread_mutex_unlock(&players[id].pos_mutex_w);
	pthread_mutex_unlock(&players[id].pos_mutex_c);

	if (px                 < x + w
	 && px + PLAYER_WIDTH  > x
	 && py                 < y + h
	 && py + PLAYER_HEIGHT > y)
		return 1;
	
	return 0;
}