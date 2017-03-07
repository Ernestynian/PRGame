#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "../../Common/networkInterface.h"

#include "Renderer.h"
#include "Map.h"
#include "Player.h"


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
	bool isIdCorrect(char id);
	
	const int gravity;
	Map* map;
	
	Renderer* renderer;
	
	Texture* playerTexture;
	std::vector<Player*> players; // Players connected to server 
	Player** playersById;        // Players "sorted" by their server id
	int selfID;
};

#endif /* WORLD_H */

