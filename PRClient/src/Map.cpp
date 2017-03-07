#include <SDL2/SDL.h>

#include "Map.h"


Map::Map(Renderer* renderer) {
	Texture* t = new Texture(renderer, "res/background.png");
	background = new Sprite(t, 0, 0, renderer->width, renderer->height);
	
	playerSpace.x = 0;
	playerSpace.y = 0;
	playerSpace.w = renderer->width;
	playerSpace.h = renderer->height - 30;
}


Map::~Map() {
	delete background;
}


bool Map::canFall(SDL_Rect object) {
	if (object.y + object.h + 1 > playerSpace.x + playerSpace.h)
		return false;
	
	return true;
}


bool Map::collides(SDL_Rect* object) {
	if (object->y + object->h > playerSpace.x + playerSpace.h) {
		object->y = playerSpace.x + playerSpace.h - object->h;
		return true;
	}
	
	return false;
}


void Map::draw() {
	background->draw();
}