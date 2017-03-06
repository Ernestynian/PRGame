#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "Renderer.h"
#include "Sprite.h"
#include "Player.h"

#include "../../Common/networkInterface.h"

class World {
public:
	World(Renderer* renderer, unsigned int selfID);
	~World();
	
	void update();
	void draw();
	
	void addPlayer(int id);
	void removePlayer(int id);
	
	void parseEvent(EventTypes type, uint8_t* data);
	
private:
	Renderer* renderer;
	
	Sprite* background;
	Texture* playerTexture;
	std::vector<Player*> players; // Players connected to server 
	Player** playersById;        // Players "sorted" by their server id
	int selfID;
};

#endif /* WORLD_H */

