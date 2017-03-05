#include "Game.h"

#include "../../Common/byteConverter.h"

#define DESIRED_FRAMERATE 60


Game::Game(int argc, const char* argv[]) : msPerFrame(1000 / DESIRED_FRAMERATE), 
										   networkTickrate(30) {	
	video   = new Video();
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
	delete video;
}


int Game::run() {
	if (video->failed())
		return -1;
	
	network = new Network();
	
	makeConnection();
	
	// network creates the world
	world = new World();
	
	while (running) {
		int startTime = SDL_GetTicks();
		
		processEvents();
		
		network->checkForOneNewPacket();
		
		world->update();
		
		network->sendPacket(frame);
		
		video->render();
		
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
 */
void Game::makeConnection() {
	int timeoutStart = SDL_GetTicks();
	while(running) {
		processEvents();
		
		// Normally there should be also NEGATIVE response and so repetitive
		//  JOIN messages, but we won't go into such lengths. Let's just
		//  try once and if no response - end the suffering
		if (network->recieviedAcceptMessage())
			break;
		else if (SDL_GetTicks() - timeoutStart > MS_TO_TIMEOUT) {
			video->showError("Network error.", "Server did not respond.");
			running = false;
			break;
		}
		
		SDL_Delay(1);
	}
}


void Game::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT)
			running = false;
		// Test events TODO: remove
		else if (e.type == SDL_KEYDOWN) {
			network->addNewEvent(NET_EVENT_PLAYER_MOVED, "44", 8, 123);
		} else if (e.type == SDL_KEYUP) {
			network->addNewEvent(NET_EVENT_PLAYER_LEFT, "world", 6);
		}
	}
}