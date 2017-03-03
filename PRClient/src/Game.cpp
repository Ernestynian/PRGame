#include "Game.h"

#include "../../Common/byteConverter.h"

#define DESIRED_FRAMERATE 60


Game::Game(int argc, const char* argv[]) : msPerFrame(1000 / DESIRED_FRAMERATE), 
										   networkTickrate(30) {	
	video = new Video();
	world = nullptr;
	
	running = true;
	
	frame = 0;
}


Game::~Game() {
	if (world != nullptr)
		delete world;
	delete video;
}


int Game::run() {
	if (video->failed())
		return -1;
	
	world = new World();
	network = new Network();
	
	makeConnection();
	
	while (running) {
		int startTime = SDL_GetTicks();
		
		processEvents();
		
		network->checkForData();
		
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
 * Make sure the server is running and has slot for us
 */
void Game::makeConnection() {
	int timeoutStart = SDL_GetTicks();
	while(running) {
		processEvents();
		
		if (network->checkIfHasBeenAccepted())
			break;
		else if (SDL_GetTicks() - timeoutStart > MS_TO_TIMEOUT) {
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
			char* bytes = toBytes("44", 15, 260);
			network->addNewEvent(NET_EVENT_PLAYER_MOVED, bytes, 8);
			free(bytes);
		} else if (e.type == SDL_KEYUP) {
			network->addNewEvent(NET_EVENT_PLAYER_LEFT, "world", 6);
		}
	}
}