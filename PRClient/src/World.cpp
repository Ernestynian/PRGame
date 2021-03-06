#include <stdexcept>

#include <SDL2/SDL.h>

#include "../../Common/byteConverter.h"

#include "Texture.h"
#include "World.h"


World::World(Renderer* renderer, unsigned int selfID, std::vector<IconData*> mapIcons)
	: gravity(GRAVITY_CONSTANT) {
	this->renderer = renderer;
	this->selfID   = selfID;
	
	map = new Map(renderer, mapIcons);
	
	playersById = new Player*[MAX_CLIENTS];
	for (int i = 0; i < MAX_CLIENTS; ++i)
		playersById[i] = nullptr;
	
	playerBodyTexture = renderer->createTexture("player_body_sprites.png");
    playerHandsTexture = renderer->createTexture("player_hands_sprites.png");
	
	selfLeftDirection = selfRightDirection = false;
}


World::~World() {
	for (auto player : players)
		delete player;
	delete[] playersById;
	players.clear();
	
	delete map;
}


void World::update(float delta) {
	if (playersById[selfID] != nullptr
	 && playersById[selfID]->isAlive()) {
		if (selfLeftDirection == selfRightDirection)
			playersById[selfID]->applyFriction(PLAYER_ACCELERATION);
		else if (selfLeftDirection)
			playersById[selfID]->addSpeed(-PLAYER_ACCELERATION);
		else if (selfRightDirection)
			playersById[selfID]->addSpeed(PLAYER_ACCELERATION);
		
#ifdef DEBUG
		//playersById[selfID]->printStatus();
#endif
	}
	
	for (Player* player : players) {		
		if (player->isDying())
			player->applyFriction(0.002);
		
		if (player->isAlive() || player->isDying()) {
			player->applyGravity(map, gravity * delta);
			player->move(map, delta);
			player->calculateAnimation(delta);
		}
    }
}


void World::draw() {
	renderer->clear();
	
	map->draw();
	
	for (Player* player : players)
		if (player->isAlive() || player->isDying())
			player->draw();
	
	renderer->render();
}


void World::addPlayer(int id) {
	if (playersById[id] != nullptr)
		removePlayer(id);
	
	players.push_back(new Player(playerBodyTexture, playerHandsTexture));
	playersById[id] = players.back();
}


void World::removePlayer(int id) {
	for(auto it = players.begin(); it != players.end(); ++it) {
		if (*it == playersById[id]) {
			delete *it;
			players.erase(it);
			break;
		}
	}
	playersById[id] = nullptr;
}


void World::parseEvent(EventTypes type, uint8_t* data) {
	switch (type) {
		case NET_EVENT_PLAYER_SPAWN: {
			char id = binaryRead1B();
			float x = binaryReadFloat();
			float y = binaryReadFloat();
			if (isIdCorrect(id)) {
				printf("NET_EVENT_PLAYER_SPAWN %hhu %.0f %.0f\n", id, x, y);
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
			char direction = binaryRead1B();
			if (isIdCorrect(id)) {
				if (playersById[id] != nullptr)
					playersById[id]->kill(direction);
				else
					printf("%d is nullptr!!!!\n", id);
			}
			
			break;
		}
		case NET_EVENT_PLAYER_MOVED: {
			char id  = binaryRead1B();
			float x  = binaryReadFloat();
			float y  = binaryReadFloat();
			float vx = binaryReadFloat();
			float vy = binaryReadFloat();
			if (isIdCorrect(id)) {
				if (id != selfID) {
					if (!playersById[id]->isDying()) {
						playersById[id]->teleportToPosition(x, y);
						playersById[id]->setSpeed(vx, vy);
					}
				}
			}
			
			break;
		}
		case NET_EVENT_PLAYER_MOVE_DENIED: {
			printf("NET_EVENT_PLAYER_MOVE_DENIED\n");
			char id  = binaryRead1B();
			float x  = binaryReadFloat();
			float y  = binaryReadFloat();
			if (isIdCorrect(id)) {
				if (id == selfID)
					playersById[id]->teleportToPosition(x, y);
			}
			
			break;
		}
		case NET_EVENT_PLAYER_JUMP: {
			char id = binaryRead1B();
			if (isIdCorrect(id)) {
				if (id != selfID)
					playersById[id]->tryToJump(map, JUMP_ACCELERATION);
			}
			
			break;
		}
        case NET_EVENT_PLAYER_ATTACK: {
			char id = binaryRead1B();
			if (isIdCorrect(id)) {
				if (id != selfID)
					playersById[id]->attack();
			}
			
			break;
		}
	}
}


///////////////////////
// CONTROLLER PLAYER //
///////////////////////

void World::selfStartMoving(PlayerDirections direction) {
	if (direction == DIRECTION_LEFT)
		selfLeftDirection = true;
	
	if (direction == DIRECTION_RIGHT)
		selfRightDirection = true;
}


void World::selfStopMoving(PlayerDirections direction) {
	if (direction == DIRECTION_LEFT)
		selfLeftDirection = false;
	
	if (direction == DIRECTION_RIGHT)
		selfRightDirection = false;
}


/**
 * Try to perform jump of the controlled player
 * @return true when acually jumped
 */
bool World::selfJump() {
	return playersById[selfID]->tryToJump(map, JUMP_ACCELERATION);		
}


bool World::selfAttack() {
    return playersById[selfID]->attack();
}


bool World::selfHasMoved() {
	if (playersById[selfID] == nullptr)
		return false;
	
	return playersById[selfID]->hasMoved();
}


bool World::selfStopped() {
	if (playersById[selfID] == nullptr)
		return false;
	
	return playersById[selfID]->hasStopped();
}


float World::getSelfPosX() {
	return playersById[selfID]->getPosX();
}


float World::getSelfPosY() {
	return playersById[selfID]->getPosY();
}


float World::getSelfSpeedX() {
	return playersById[selfID]->getSpeedX();
}


float World::getSelfSpeedY() {
	return playersById[selfID]->getSpeedY();
}


bool World::isIdCorrect(char id) {
	if (id >= 0 && id < MAX_CLIENTS)
		return true;
	else
		throw std::runtime_error(std::string("received id not in the correct range"));
}
