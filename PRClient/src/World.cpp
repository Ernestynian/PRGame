#include <stdexcept>

#include <SDL2/SDL.h>

#include "../../Common/byteConverter.h"

#include "Texture.h"
#include "World.h"


World::World(Renderer* renderer, unsigned int selfID) : gravity(0.0054) {
	this->renderer = renderer;
	this->selfID   = selfID;
	
	map = new Map(renderer);
	
	playersById = new Player*[MAX_CLIENTS];
	for (int i = 0; i < MAX_CLIENTS; ++i)
		playersById[i] = nullptr;
	
	playerTexture = new Texture(renderer, "res/player_sprites.png");
	
	selfDirection = DIRECTION_NONE;
	
	//players.push_back(Player(playerTexture));
	//playersById[selfID] = &players.back();
	
}


World::~World() {
	for (auto player : players)
		delete player;
	delete[] playersById;
	players.clear();
	
	delete map;
}


void World::update(float delta) {
	for (auto player : players)
		player->applyGravity(map, gravity * delta);
	
	if (playersById[selfID] != nullptr) {
		if (playersById[selfID]->isAlive()) {
			if (selfDirection != DIRECTION_NONE) {
				playersById[selfID]->setSpeed(0.1 * delta * selfDirection, 0);
			}
		}
	}
	
	for (auto player : players)
		player->move(map);
}


void World::draw() {
	renderer->clear();
	
	map->draw();
	
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
			int x   = binaryRead4B();
			int y   = binaryRead4B();
			if (isIdCorrect(id)) {
				if (playersById[id] != nullptr)
					playersById[id]->spawn(x, y);
				else
					printf("%d is nullptr!!!!\n", id);
			}
				
			break;
		}
		case NET_EVENT_PLAYER_DIED: {
			printf("NET_EVENT_PLAYER_DIED\n");
			char id = binaryRead1B();
			if (isIdCorrect(id)) {
				if (playersById[id] != nullptr)
					playersById[id]->kill();
				else
					printf("%d is nullptr!!!!\n", id);
			}
			
			break;
		}
		case NET_EVENT_PLAYER_MOVED: {
			char id = binaryRead1B();
			int x   = binaryRead4B();
			int y   = binaryRead4B();
			if (isIdCorrect(id)) {
				if (id != selfID)
					playersById[id]->teleportToPosition(x, y);
			}
			
			break;
		}
	}
}


void World::selfStartMoving(int direction) {
	selfDirection = (PlayerDirections)direction;
}


void World::selfStopMoving(int direction) {
	if (selfDirection == direction) {
		selfDirection = DIRECTION_NONE;
		if (playersById[selfID]->canMove())
			playersById[selfID]->setSpeed(0, 0);
	}
}


/**
 * Try to perform jump of the controlled player
 * @return true when acually jumped
 */
bool World::selfJump() {
	return playersById[selfID]->tryToJump(-5);		
}


bool World::selfHasMoved() {
	if (playersById[selfID] == nullptr)
		return false;
	
	return playersById[selfID]->hasMoved();
}


int World::getSelfPosX() {
	return playersById[selfID]->getPosX();
}


int World::getSelfPosY() {
	return playersById[selfID]->getPosY();
}


bool World::isIdCorrect(char id) {
	if (id >= 0 && id < MAX_CLIENTS)
		return true;
	else
		throw std::runtime_error(std::string("received id not in the correct range"));
}
