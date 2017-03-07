#include <SDL2/SDL.h>

#include "Map.h"


Map::Map(Renderer* renderer) {
	Texture* t = new Texture(renderer, "res/background.bmp");
	background = new Sprite(t, 0, 0, renderer->width, renderer->height);
	
	playerSpace.x = 0;
	playerSpace.y = 0;
	playerSpace.w = renderer->width;
	playerSpace.h = renderer->height;
}


Map::~Map() {
	delete background;
}


bool Map::collides(SDL_Rect object) {
	if (object.y + object.h > playerSpace.x + playerSpace.h)
		return true;
	
	return false;
}


void Map::draw() {
	background->draw();
}