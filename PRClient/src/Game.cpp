#include <SDL2/SDL.h>
#include "Renderer.h"
#include "Window.h"
#include "Network.h"
#include "World.h"
#include "Game.h"

#include "../../Common/networkInterface.h"
#include "../../Common/byteConverter.h"


Game::Game(int argc, const char* argv[])
: msPerFrame(1000.0 / FRAMERATE), networkTickrate(30) {	
	window  = new Window();
	world   = nullptr;
	network = nullptr;
	
	running = true;
	
	frame   = 0;
}


Game::~Game() {
	if (network != nullptr)
		delete network;
	if (world != nullptr)
		delete world;
	delete window;
}


int Game::run() {
	if (window->failed())
		return -1;
	
	network = new Network();
	
	std::pair<int, std::vector<IconData*>> worldData = makeConnection();
	
	// network creates the world
	if (worldData.first >= 0)
		world = new World(window->getRenderer(), worldData.first, 
												 worldData.second);
	
	while (running) {
		int startTime = SDL_GetTicks();
		
		processEvents();
		
		checkPackets();
		
		world->update(msPerFrame); // TODO: make it dynamic (delta)
		
		if (world->selfHasMoved()
		 || world->selfStopped()) {
			network->addNewEvent(NET_EVENT_PLAYER_MOVED, "ffff", 
					world->getSelfPosX(),   world->getSelfPosY(),
					world->getSelfSpeedX(), world->getSelfSpeedY());
		}
		
		network->sendPacket(frame);
		
		world->draw();
		
		int delta = SDL_GetTicks() - startTime;
		
		// Upper bound of FPS
		if (msPerFrame > delta)
			SDL_Delay((int)msPerFrame - delta);
		
		frame++;
		if (frame == 0)
			frame++;
	}
	
	return 0;
}


/**
 * Make sure the server is running and has a slot for us
 * @return player id
 */
std::pair<int, std::vector<IconData*>> Game::makeConnection() {
	std::pair<int, std::vector<IconData*>> data;
	data.first = -1;
	
	int timeoutStart = SDL_GetTicks();
	while(running) {
		processEvents();
		
		// Normally there should be also NEGATIVE response and so repetitive
		//  JOIN messages, but we won't go into such lengths. Let's just
		//  try once and if no response - end the suffering
		if (network->recieviedAcceptMessage()) {
			if (network->getCurrentEventDataLength() > 0) {
				initBinaryReader((char*)network->getCurrentEventData());
				char id = binaryRead1B();
				if (id >= 0 && id <= MAX_CLIENTS) {
					data.first = id;
					char iconsAmount = binaryRead1B();
					for (int i = 0; i < iconsAmount; ++i) {
						IconData* icon = new IconData();
						icon->x = binaryRead2B();
						icon->y = binaryRead2B();
						icon->textureId = binaryRead1B();
						data.second.push_back(icon);
					}
					
					return data;
				}
			}
			window->showError("Network error.", "Failed to make a connection with server.");
			break;
		} else if (SDL_GetTicks() - timeoutStart > MS_TO_TIMEOUT) {
			window->showError("Network error.", "Server did not respond.");
			break;
		}
		
		SDL_Delay(1);
	}
	
	running = false;
	return data;
}


void Game::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT)
			running = false;
		else if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
				case SDLK_RIGHT:
				case SDLK_d:
					world->selfStartMoving(DIRECTION_RIGHT);
					break;
				case SDLK_LEFT:
				case SDLK_a:
					world->selfStartMoving(DIRECTION_LEFT);
					break;
                case SDLK_f:
                    if(world->selfAttack())
                        network->addNewEvent(NET_EVENT_PLAYER_ATTACK);
                    break;
				case SDLK_SPACE:
				case SDLK_UP:
					if (world->selfJump())
						network->addNewEvent(NET_EVENT_PLAYER_JUMP);
					break;
			}		
		} else if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
				case SDLK_RIGHT:
				case SDLK_d:
					world->selfStopMoving(DIRECTION_RIGHT);
					break;
				case SDLK_LEFT:
				case SDLK_a:
					world->selfStopMoving(DIRECTION_LEFT);
					break;
			}			
		}
	}
}


void Game::checkPackets() {
	if (network->receivePacket()) {
		for(;;) {
			switch (network->getCurrentEventDataType()) {
			case NET_EVENT_CLIENT_JOIN: {
				int pid = binaryRead1B();
				world->addPlayer(pid);
				printf("Added client: %d\n", pid);
				break;
			}
			case NET_EVENT_CLIENT_EXIT: {
				int pid = binaryRead1B();
				world->removePlayer(pid);
				printf("Removed client: %d\n", pid);
				break;
			}
			default:
				world->parseEvent(network->getCurrentEventDataType(), 
								  network->getCurrentEventData());
				break;
			}
			
			if (network->isThereMoreEvents())
				network->getNextEvent();
			else
				break;
		};
	}
}