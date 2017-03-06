#include <SDL2/SDL.h>
#include "Renderer.h"
#include "Window.h"
#include "Network.h"
#include "World.h"
#include "Game.h"

#include "../../Common/networkInterface.h"
#include "../../Common/byteConverter.h"

#define DESIRED_FRAMERATE 60


Game::Game(int argc, const char* argv[]) : msPerFrame(1000 / DESIRED_FRAMERATE), 
										   networkTickrate(30) {	
	window = new Window();
	world    = nullptr;
	network  = nullptr;
	
	running  = true;
	
	frame    = 0;
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
	
	int id = makeConnection();
	
	// network creates the world
	if (id >= 0)
		world = new World(window->getRenderer(), id);
	
	while (running) {
		int startTime = SDL_GetTicks();
		
		processEvents();
		
		checkPackets();
		
		world->update();
		
		network->sendPacket(frame);
		
		world->draw();
		
		int delta = startTime - SDL_GetTicks();
		
		// Upper bound of FPS
		if (msPerFrame > delta)
			SDL_Delay(msPerFrame - delta);
		
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
int Game::makeConnection() {
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
				if (id >= 0 && id <= MAX_CLIENTS)
					return id;
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
	return -1;
}


void Game::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT)
			running = false;
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