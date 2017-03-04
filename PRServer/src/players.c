#include <stdio.h>
#include <pthread.h>
#include "players.h"

#include "../../Common/networkInterface.h"


player players[MAX_CLIENTS];


void players_init() {
	for (int i = 0; i < MAX_CLIENTS; ++i)
		pthread_mutex_init(&players[i].mutex, NULL);
}


void player_reset(int id) {
	pthread_mutex_lock(&players[id].mutex);
	players[id].pos_x = 0;
	players[id].pos_y = 0;
	players[id].alive = 0;
	players[id].moved = 0;
	pthread_mutex_unlock(&players[id].mutex);
}

void player_moved(int id, char* data) {
	initBinaryReader(data);
	int newX = binaryRead4B();
	int newY = binaryRead4B();
	
	// Check collisions
	
	pthread_mutex_lock(&players[id].mutex);
	players[id].pos_x = newX;
	players[id].pos_y = newY;
	players[id].moved = 1;
	pthread_mutex_unlock(&players[id].mutex);
}