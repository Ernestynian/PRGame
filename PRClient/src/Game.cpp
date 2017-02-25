#include "Game.h"

Game::Game() {
	window = nullptr;
}

Game::~Game() {
	if (window != nullptr)
		SDL_DestroyWindow(window);
	SDL_Quit();
}

int Game::run(int argc, char* argv[]) {
	// TODO: create Video class, or something similar
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;
	
	window = SDL_CreateWindow("SDL 2.0 Test",
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				800, 600, SDL_WINDOW_SHOWN);
	
	SDL_Delay(3000);
	
	return 0;
}