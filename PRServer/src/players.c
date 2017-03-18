#include <stdio.h>
#include <pthread.h>

#include "../../Common/networkInterface.h"

#include "players.h"

player players[MAX_CLIENTS];


void players_init() {
	for (int i = 0; i < MAX_CLIENTS; ++i)
		pthread_mutex_init(&players[i].mutex, NULL);
}


void player_reset(int id) {
	pthread_mutex_lock(&players[id].mutex);
	players[id].pos.x   = 0;
	players[id].pos.y   = 0;
	players[id].speed.x = 0;
	players[id].speed.y = 0;
	players[id].alive   = 0;
	players[id].moved   = 0;
	pthread_mutex_unlock(&players[id].mutex);
}


int player_moved(char id, float x, float y, float vx, float vy) {
	// TODO: Check collisions
	// TODO: verify if this move can be accepted
	
	pthread_mutex_lock(&players[id].mutex);
	players[id].pos.x   = x;
	players[id].pos.y   = y;
	players[id].speed.x = vx;
	players[id].speed.y = vy;
	players[id].moved   = 1;
	pthread_mutex_unlock(&players[id].mutex);
	
	return 1;
}


void player_getPos(char id, float* x, float* y) {
	pthread_mutex_lock(&players[id].mutex);
	*x = players[id].pos.x;
	*y = players[id].pos.y;
	pthread_mutex_unlock(&players[id].mutex);
}


void player_spawn(int id, float x, float y) {
	pthread_mutex_lock(&players[id].mutex);
	players[id].pos.x = x;
	players[id].pos.y = y;
	players[id].alive = 1;
	pthread_mutex_unlock(&players[id].mutex);
}


void player_kill(int id) {
	pthread_mutex_lock(&players[id].mutex);
	players[id].alive = 0;
	pthread_mutex_unlock(&players[id].mutex);	
}