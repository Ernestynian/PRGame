#ifndef PLAYER_H
#define PLAYER_H

#include "Map.h"

enum PlayerState {
	PLAYER_STILL,
	PLAYER_MOVING,
	PLAYER_FALLING,
	PLAYER_JUMPING,
	PLAYER_CROUCHING
};

class Player {
public:
	Player(Texture* texture);
	virtual ~Player();
	
	void spawn(int x, int y);
	void kill();
	void move(int x, int y);
	
	void applyGravity(Map* map, int g);
	
	void draw();
	
	bool isAlive();
	
private:
	int x;
	int y;
	
	bool alive;
	
	Texture* texture;
};

#endif /* PLAYER_H */

