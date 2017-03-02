#include "Game.h"

#include "../../Common/byteConverter.h"

Game::Game(int argc, const char* argv[]) : msPerFrame(1000 / 60), 
										   networkTickrate(30) {	
	video = new Video();
	world = new World();
	
	frame = 0;
}


Game::~Game() {
	delete world;
	delete video;
}


int Game::run() {
	if (video->failed())
		return -1;
	
	running = true;
	
	while (running) {
		int startTime = SDL_GetTicks();
		
		processEvents();
		
		network.checkForData();
		
		world->update();
		
		network.sendPacket(frame);
		
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


void Game::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT)
			running = false;
		// Test events TODO: remove
		else if (e.type == SDL_KEYDOWN) {
			char* bytes = toBytes("44", 15, 260);
			network.addEvent(EVENT_PLAYER_MOVED, bytes, 8);
			free(bytes);
		} else if (e.type == SDL_KEYUP) {
			network.addEvent(EVENT_PLAYER_LEFT, "world", 6);
		}
	}
}