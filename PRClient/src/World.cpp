#include <SDL2/SDL.h>

#include "Texture.h"
#include "World.h"

#include "../../Common/networkInterface.h"
#include "../../Common/byteConverter.h"


World::World(Renderer* renderer, unsigned int selfID) {
	this->renderer = renderer;
	this->selfID   = selfID;
	
	Texture* t = new Texture(renderer, "res/background.bmp");
	background = new Sprite(t, 0, 0);
	
	playersById = new Player*[MAX_CLIENTS];
	for (int i = 0; i < MAX_CLIENTS; ++i)
		playersById[i] = nullptr;
	
	playerTexture = new Texture(renderer, "res/player.bmp");
	
	//players.push_back(Player(playerTexture));
	//playersById[selfID] = &players.back();
	
}


World::~World() {
	for (auto player : players)
		delete player;
	delete[] playersById;
	players.clear();
	
	delete background;
}


void World::update() {
	
}


void World::draw() {
	renderer->clear();
	
	background->draw();
	for (auto player : players)
		if (player->isAlive())
			player->draw();
	
	renderer->render();
}


void World::addPlayer(int id) {
	// prevent memory leak
	if (playersById[id] != nullptr)
		removePlayer(id);
	
	players.push_back(new Player(playerTexture));
	playersById[id] = players.back();
}


void World::removePlayer(int id) {
	for(auto it = players.begin(); it != players.end(); ++it) {
		if (*it == playersById[id]) {
			
			players.erase(it);
			break;
		}
	}
	playersById[id] = nullptr;
}


void World::parseEvent(EventTypes type, uint8_t* data) {
	switch (type) {
		case NET_EVENT_PLAYER_SPAWN: {
			printf("NET_EVENT_PLAYER_SPAWN\n");
			char id = binaryRead1B();
			int  x  = binaryRead4B();
			int  y  = binaryRead4B();
			
			if (id >= 0 && id <= MAX_CLIENTS)
				playersById[id]->spawn(x, y);
			else
				; // THROW
			break;
		}
		case NET_EVENT_PLAYER_DIED:
			break;
		case NET_EVENT_PLAYER_MOVED:
			break;
	}
}