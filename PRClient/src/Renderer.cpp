#include <SDL2/SDL.h>

#include "Renderer.h"


Renderer::Renderer(SDL_Window* window) {
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}


Renderer::~Renderer() {
	if (renderer != nullptr) {
		for (auto texture : textures)
			SDL_DestroyTexture(texture);
		textures.clear();
		
		SDL_DestroyRenderer(renderer);
	}
}


void Renderer::clear() {
	SDL_RenderClear(renderer);
}


void Renderer::render() {
	SDL_RenderPresent(renderer);
}


SDL_Texture* Renderer::createTextureFromSurface(SDL_Surface* surface) {
	SDL_Texture* temp = SDL_CreateTextureFromSurface(renderer, surface);
	textures.push_back(temp);
	return temp;
}

void Renderer::draw(SDL_Texture* texture, SDL_Rect* sourceQuad, SDL_Rect* renderQuad) {	
	SDL_RenderCopy(renderer, texture, sourceQuad, renderQuad);
}