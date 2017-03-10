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
	Player(Texture* bodyTexture, Texture* handsTexture);
	virtual ~Player();
	
	void spawn(int x, int y);
	void kill();
	void teleportToPosition(int x, int y);
	
	void applyGravity(Map* map, float g);
	void setSpeed(float x, float y);
	
	bool tryToJump(float speed);
	
	void move(Map* map, float delta);
	bool hasMoved();
	
	void calculateAnimation(float delta);
	void draw();
	
	bool isAlive();
	bool canMove();
	
	float getPosX();
	float getPosY();
	float getSpeedX();
	float getSpeedY();
	
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
        
        float deltaAnimTime;
        double animCycleTime;//const
        int animFrameCount;//const
        SDL_RendererFlip flip;
        int bodyAnimFrame;
        int handsAnimFrame;
	
	bool alive;
	
	Texture* bodyTexture;
        Texture* handsTexture;
};

#endif /* PLAYER_H */

