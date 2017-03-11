#include <SDL2/SDL.h>

#include "Window.h"

#include "Renderer.h"


Window::Window() {
	window = nullptr;
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return;
	
	window = SDL_CreateWindow("Game",
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				getScreenWidth(), getScreenHeight(), SDL_WINDOW_SHOWN);
	
	renderer = new Renderer(window, getScreenWidth(), getScreenHeight());
}


Window::~Window() {
	delete renderer;
	
	if (window != nullptr)
		SDL_DestroyWindow(window);
	SDL_Quit();
}


int Window::getScreenWidth() {
	return 800;	
}


int Window::getScreenHeight() {
	return 600;
}


bool Window::failed() {
	return window == nullptr;
}


Renderer* Window::getRenderer() {
	return renderer;
}


void Window::showError(const char* title, const char* message) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, window);
}