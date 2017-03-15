#include <SDL2/SDL.h>

#include "Map.h"


Map::Map(Renderer* renderer, std::vector<IconData*> newIcons) {
	Texture* t = new Texture(renderer, "res/background.png");
	background = new Sprite(t, 0, 0, renderer->width, renderer->height);
	
	playerSpace.x = 0;
	playerSpace.y = 0;
	playerSpace.w = renderer->width;
	playerSpace.h = renderer->height - 30;

	Texture* icTx[] = {
		new Texture(renderer, "res/icons/My Computer.png"),
		new Texture(renderer, "res/icons/My Documents.png"),
		new Texture(renderer, "res/icons/My Music.png"),
		new Texture(renderer, "res/icons/My Pictures.png"),
		new Texture(renderer, "res/icons/My Videos.png"),
		new Texture(renderer, "res/icons/Mozilla Firefox.png"),
		new Texture(renderer, "res/icons/Mozilla Thunderbird.png"),
		new Texture(renderer, "res/icons/Media Player Classic.png")
		//new Texture(renderer, "res/icons/.png")
	};
	
	int icTxLength = sizeof(icTx) / sizeof(icTx[0]);
	
	for (IconData* newIcon : newIcons) {
		if (newIcon->textureId >= 0
		 && newIcon->textureId < icTxLength) {
			Icon* i = new Icon(icTx[newIcon->textureId], 
							newIcon->x, newIcon->y, 32);
			icons.push_back(i);
		}
		
		delete newIcon;
	}	
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


CollisionSide Map::collides(int x, int y, int w, int h) {
	for (Icon* icon : icons) {
		if (icon->collides(x, y, w, h)) {
			float hx = (h + 32) * ((x + w * 0.5) - icon->getCenterX());
			float wy = (w + 32) * ((y + h * 0.5) - icon->getCenterY());
			if (wy > hx) {
				if (wy > -hx)
					return CollidedBottom;
				else
					return CollidedLeft;
			} else {
				if (wy > -hx)
					return CollidedRight;
				else
					return CollidedTop;
			}
		}
	}
	
	return NotCollided;
}


void Map::draw() {
	background->draw();
	
	// TODO: move to different function that will be executed after the players
	for (auto icon : icons)
		icon->draw();
}