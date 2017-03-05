#include <SDL2/SDL.h>

#include "Texture.h"


Texture::Texture(Renderer* renderer, const char* filename) {
	this->renderer = renderer;
	
	SDL_Surface* surface = SDL_LoadBMP(filename);
	//if (transparentBackground)
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
	texture = renderer->createTextureFromSurface(surface);
    SDL_FreeSurface(surface);
	
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	
	SDL_QueryTexture(texture, 0, 0, &width, &height);
}


Texture::~Texture() {
	
}


int Texture::getWidth() {
	return width;
}


int Texture::getHeight() {
	return height;
}


void Texture::draw(SDL_Rect* renderQuad) {
	renderer->draw(texture, renderQuad);
}