#include <SDL2/SDL.h>

#include "Map.h"


Map::Map(Renderer* renderer) {
	Texture* t = new Texture(renderer, "res/background.png");
	background = new Sprite(t, 0, 0, renderer->width, renderer->height);
	
	playerSpace.x = 0;
	playerSpace.y = 0;
	playerSpace.w = renderer->width;
	playerSpace.h = renderer->height - 30;

	t = new Texture(renderer, "res/icons/My Computer.png");
	Icon* i = new Icon(t, 140, 480, 32);
	icons.push_back(i);
	i = new Icon(t, 340, 400, 32);
	icons.push_back(i);
}


Map::~Map() {
	delete background;
	for (auto icon : icons)
		delete icon;
	icons.clear();
}


bool Map::canFall(SDL_Rect object) {
	if (object.y + object.h + 1 > playerSpace.x + playerSpace.h)
		return false;
	
	for (auto icon : icons) {
		if (icon->downCollision(object.x, object.y, object.w, object.h))
			return false;
	}
	
	return true;
}



/**
 * Check if a square object horizontally intersects with map objects
 * @param object position, changed to new position when collided
 * @return true when collides
 */
bool Map::hcollides(int* x, int w) {
	// left
	if (*x < playerSpace.x) {
		*x = playerSpace.x;
		return true;
	}
	
	// right
	if (*x + w > playerSpace.x + playerSpace.w) {
		*x = playerSpace.x + playerSpace.w - w;
		return true;
	}
	
	return false;
}


/**
 * Check if a square object vertically intersects with map objects
 * @param object position, changed to new position when collided
 * @return true when collides
 */
bool Map::vcollides(int* y, int h) {
	// bottom
	if (*y + h > playerSpace.y + playerSpace.h) {
		*y = playerSpace.y + playerSpace.h - h;
		return true;
	}
	
	return false;
}


bool Map::collides(int x, int y, int w, int h) {
	for (auto icon : icons) {
		if (icon->collides(x, y, w, h))
			return true;
	}
	
	return false;
}


void Map::draw() {
	background->draw();
	
	// TODO: move to different function that will be executed after the players
	for (auto icon : icons)
		icon->draw();
}