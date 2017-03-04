#ifndef PLAYERS_H
#define PLAYERS_H

typedef struct {
	pthread_mutex_t mutex;
	
	int pos_x, pos_y;
	char alive;
	
	char moved;
} player;


void players_init();

void player_reset(int id);

void player_moved(int id, char* data);

#endif /* PLAYERS_H */

