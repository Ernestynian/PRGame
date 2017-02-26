#include "Game.h"

Game::Game(int argc, const char* argv[]) : ticksPerFrame(16) {	
	video = new Video();
}


Game::~Game() {
	delete video;
}


int Game::run() {
	if (video->failed())
		return -1;
	
	running = true;
	lastTime = SDL_GetTicks();
	
	while (running) {
		processEvents();
		
		currentTime = SDL_GetTicks();
		int delta = currentTime - lastTime;
		lastTime = currentTime;

		//world->update(delta);
		
		video->render();
		
		// Upper bound of FPS
		if (delta > ticksPerFrame)
			SDL_Delay(delta - ticksPerFrame);
	}
	
	return 0;
}


void Game::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent( &e ) != 0) {
		if (e.type == SDL_QUIT)
			running = false;
	}
}