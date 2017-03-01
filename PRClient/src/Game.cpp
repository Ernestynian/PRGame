#include "Game.h"

#include "../../Common/byteConverter.h"

Game::Game(int argc, const char* argv[]) : msPerFrame(1000 / 60), 
										   networkTickrate(30) {	
	video = new Video();
}


Game::~Game() {
	delete video;
}


int Game::run() {
	if (video->failed())
		return -1;
	
	running = true;
	
	int frames = 0;
	while (running) {
		int startTime = SDL_GetTicks();
		
		network.checkForData();
		processEvents();
		network.sendPacket();

		//world->update(delta);
		
		video->render();
		
		int delta = startTime - SDL_GetTicks();
		
		// Upper bound of FPS
		if (msPerFrame > delta)
			SDL_Delay(msPerFrame - delta);
	}
	
	return 0;
}


void Game::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT)
			running = false;
		else if (e.type == SDL_KEYDOWN) {
			short he = 'h';
			he = he << 8 + 'e';
			network.addEvent(EVENT_PLAYER_MOVED, toBytes("44", 15, 260), 8);
		} else if (e.type == SDL_KEYUP) {
			network.addEvent(EVENT_PLAYER_LEFT, "world", 6);
		}
	}
}