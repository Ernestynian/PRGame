#include "players.h"

#include "../../Common/networkInterface.h"

player players[MAX_CLIENTS];

void player_reset(int id) {
	players[id].pos_x = 0;
	players[id].pos_y = 0;
	players[id].alive = 0;
	players[id].moved = 0;
}