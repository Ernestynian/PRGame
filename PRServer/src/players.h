#ifndef PLAYERS_H
#define PLAYERS_H

typedef struct {
	int pos_x, pos_y;
	char alive;
	
	char moved;
} player;

void player_reset(int id);

#endif /* PLAYERS_H */

