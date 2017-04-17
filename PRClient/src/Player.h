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

enum HandsAnimation {
    RUNNING,
    RUNNING_AND_ATTACKING,
    STILL_AND_ATTACKING,
    DYING
};

class Player {
public:
	Player(Texture* bodyTexture, Texture* handsTexture);
	virtual ~Player();
	
	void spawn(int x, int y);
	void kill(char direction);
	void teleportToPosition(int x, int y);
	void setSpeed(float vx, float vy);
	
	void applyGravity(Map* map, float g);
	void addSpeed(float x);
	void applyFriction(float x);
	
	bool tryToJump(Map* map, float speed);
	
	void move(Map* map, float delta);
	bool hasMoved();
	bool hasStopped();
    
    bool attack();
	
	void calculateAnimation(float delta);
	void draw();
	
	bool isAlive();
	bool isDying();
	bool canMove();
	bool canBounce(Map* map);
	
	float getPosX();
	float getPosY();
	float getSpeedX();
	float getSpeedY();
	
	void printStatus(); // TODO: remove
	
private:
	SDL_Rect getCollisionBox();
	SDL_Rect getCollisionBox(float x_offset, float y_offset);
	void changeStateTo(PlayerState newState);
	PlayerState state;
	
	const int tileW;
	const int tileH;
	
	float x;
	float y;
	int w, h;
	
	float x_speed;
	float y_speed;
	
	float iconSideHitSpeed;
	float iconSideHitHeight;
	bool sideHit;
	bool stopped;
        
    float deltaAnimTime;
    double animCycleTime;
    int animFrameCount;
    SDL_RendererFlip flip;
    int bodyAnimFrame;
    int handsAnimFrame;
    int bodyAnimOffset;
    
    int attackAnimFrameCount;
    float attackAnimTime;
    HandsAnimation handsAnimation;
    float deltaAttackTime;
        
	
	bool alive;
	
	Texture* bodyTexture;
    Texture* handsTexture;
};

#endif /* PLAYER_H */

