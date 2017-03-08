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
	players[id].pos.x = 0;
	players[id].pos.y = 0;
	players[id].alive = 0;
	players[id].moved = 0;
	pthread_mutex_unlock(&players[id].mutex);
}


void player_moved(char id, int x, int y) {
	// Check collisions
	
	pthread_mutex_lock(&players[id].mutex);
	players[id].pos.x = x;
	players[id].pos.y = y;
	players[id].moved = 1;
	pthread_mutex_unlock(&players[id].mutex);
}


void player_spawn(int id, int x, int y) {
	pthread_mutex_lock(&players[id].mutex);
	players[id].pos.x = x;
	players[id].pos.y = y;
	players[id].alive = 1;
	pthread_mutex_unlock(&players[id].mutex);
}