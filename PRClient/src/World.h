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
	World(Renderer* renderer, unsigned int selfID);
	~World();
	
	void update(float delta);
	void draw();
	
	void addPlayer(int id);
	void removePlayer(int id);
	
	void parseEvent(EventTypes type, uint8_t* data);
	
	void selfStartMoving(int direction);
	void selfStopMoving(int direction);
	
private:
	bool isIdCorrect(char id);
	
	const float gravity;
	Map* map;
	
	Renderer* renderer;
	
	Texture* playerTexture;
	std::vector<Player*> players; // Players connected to server 
	Player** playersById;        // Players "sorted" by their server id
	
	int selfID;
	PlayerDirections selfDirection;
};

#endif /* WORLD_H */

