#include <SDL2/SDL.h>

#include "Renderer.h"


Renderer::Renderer(SDL_Window* window, int width, int height)
: width(width), height(height) {
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


void Renderer::draw(SDL_Texture* texture, SDL_Rect* clipQuad, SDL_Rect* renderQuad) {	
	SDL_RenderCopy(renderer, texture, clipQuad, renderQuad);
}

void Renderer::draw(SDL_Texture* texture, SDL_Rect* clipQuad, SDL_Rect* renderQuad, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    SDL_RenderCopyEx( renderer, texture, clipQuad, renderQuad, angle, center, flip );
}

Texture* Renderer::createTexture(const char* filename) {
    char* buffer = new char [32];
    snprintf(buffer,31,"%s%s","res/",filename);
    Texture* t = new Texture(this, buffer);
    delete [] buffer;
    return t;
}