#include <SDL2/SDL.h>

#include "Video.h"

Video::Video() {
	window = nullptr;
		
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return;
	
	window = SDL_CreateWindow("SDL 2.0 Test",
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				getScreenWidth(), getScreenHeight(), SDL_WINDOW_SHOWN);
}


Video::~Video() {
	if (window != nullptr)
		SDL_DestroyWindow(window);
	SDL_Quit();
}


int Video::getScreenWidth() {
	return 800;	
}


int Video::getScreenHeight() {
	return 600;
}


bool Video::failed() {
	return window == nullptr;
}


void Video::render() {
	//SDL_Delay(8); // Simulation
	SDL_UpdateWindowSurface(window);
}


void Video::showError(const char* title, const char* message) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, window);
}