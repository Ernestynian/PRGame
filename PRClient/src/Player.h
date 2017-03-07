#ifndef PLAYER_H
#define PLAYER_H

#include "Map.h"

enum PlayerState {
	PLAYER_STILL,
	PLAYER_MOVING,
	PLAYER_FALLING,
	PLAYER_JUMPING,
	PLAYER_CROUCHING,
	
	PLAYER_DYING
};

class Player {
public:
	Player(Texture* texture);
	virtual ~Player();
	
	void spawn(int x, int y);
	void kill();
	void moveToPosition(int x, int y);
	
	void move(Map* map);
	void setSpeed(float x, float y);
	
	void applyGravity(Map* map, float g);
	
	void draw();
	
	bool isAlive();
	bool canMove();
	
private:
	void changeStateTo(PlayerState newState);
	PlayerState state;
	
	const int tileW;
	const int tileH;
	
	float x;
	float y;
	int w, h;
	
	float x_speed;
	float y_speed;
	
	bool alive;
	
	Texture* texture;
};

#endif /* PLAYER_H */

