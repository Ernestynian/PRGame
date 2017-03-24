#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "../../Common/networkInterface.h"

#include "Renderer.h"
#include "Map.h"
#include "Player.h"

enum PlayerDirections {
	DIRECTION_NONE  =  0,
	DIRECTION_RIGHT =  1,
	DIRECTION_LEFT  = -1
};

class World {
public:
	World(Renderer* renderer, unsigned int selfID, std::vector<IconData*> mapIcons);
	~World();
	
	void update(float delta);
	void draw();
	
	void addPlayer(int id);
	void removePlayer(int id);
	
	void parseEvent(EventTypes type, uint8_t* data);
	
	void selfStartMoving(PlayerDirections direction);
	void selfStopMoving(PlayerDirections direction);
    bool selfAttack();
	bool selfJump();
	bool selfHasMoved();
	bool selfStopped();
	float getSelfPosX();
	float getSelfPosY();
	float getSelfSpeedX();
	float getSelfSpeedY();
	
private:
	bool isIdCorrect(char id);
	
	const float gravity;
	Map* map;
	
	Renderer* renderer;
	
	Texture* playerBodyTexture;
	Texture* playerHandsTexture;
	std::vector<Player*> players; // Players connected to server 
	Player** playersById;         // Players "sorted" by their server id
	
	int selfID;
	bool selfLeftDirection, selfRightDirection;
};

#endif /* WORLD_H */

