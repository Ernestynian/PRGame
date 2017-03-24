#include <stdio.h>
#include <pthread.h>

#include "../../Common/networkInterface.h"

#include "players.h"


player players[MAX_CLIENTS];


void players_init() {
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		pthread_mutex_init(&players[i].mutex, NULL);
		pthread_mutex_init(&players[i].alive_mutex, NULL);
	}
}


void player_reset(int id) {
	pthread_mutex_lock(&players[id].mutex);
	players[id].pos.x   = 0;
	players[id].pos.y   = 0;
	players[id].speed.x = 0;
	players[id].speed.y = 0;
	players[id].moved   = 0;
	pthread_mutex_unlock(&players[id].mutex);
	pthread_mutex_lock(&players[id].alive_mutex);
	players[id].alive   = 0;
	pthread_mutex_unlock(&players[id].alive_mutex);
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


void player_stopMovement(int id) {
	pthread_mutex_lock(&players[id].mutex);
	players[id].speed.x = 0;
	players[id].speed.y = 0;
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
	pthread_mutex_lock(&players[id].alive_mutex);
	players[id].alive = 0;
	pthread_mutex_unlock(&players[id].alive_mutex);
}


int player_isAlive(int id) {
	int isAlive;
	pthread_mutex_lock(&players[id].alive_mutex);
	isAlive = players[id].alive;
	pthread_mutex_unlock(&players[id].alive_mutex);
	return isAlive;
}


int player_collides(int id, int x, int y, int w, int h) {
	pthread_mutex_lock(&players[id].mutex);
	int px = players[id].pos.x;
	int py = players[id].pos.y;
	pthread_mutex_unlock(&players[id].mutex);
	if (px                 < x + w
	 && px + PLAYER_WIDTH  > x
	 && py                 < y + h
	 && py + PLAYER_HEIGHT > y)
		return 1;
	
	return 0;
}