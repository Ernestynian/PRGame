#include <SDL2/SDL.h>

#include "Map.h"


Map::Map(Renderer* renderer, std::vector<IconData*> newIcons) {
	//Texture* t = new Texture(renderer, "res/background.png");
    Texture* t = renderer->createTexture("background.png");
	background = new Sprite(t, 0, 0, renderer->width, renderer->height);
	
	playerSpace.x = 0;
	playerSpace.y = 0;
	playerSpace.w = renderer->width;
	playerSpace.h = renderer->height - 30;

	Texture* icTx[] = {
		renderer->createTexture("icons/CD Drive.png"),
		renderer->createTexture("icons/Diskette.png"),
		renderer->createTexture("icons/Folder Closed.png"),
		renderer->createTexture("icons/Folder Opened.png"),
		renderer->createTexture("icons/Hard Drive.png"),
		renderer->createTexture("icons/Media Player Classic.png"),
		renderer->createTexture("icons/Mozilla Firefox.png"),
		renderer->createTexture("icons/Mozilla Thunderbird.png"),
		renderer->createTexture("icons/My Computer.png"),
		renderer->createTexture("icons/My Documents.png"),
		renderer->createTexture("icons/My Music.png"),
		renderer->createTexture("icons/My Pictures.png"),
		renderer->createTexture("icons/My Videos.png"),
		renderer->createTexture("icons/Nero StartSmart.png"),
		renderer->createTexture("icons/Recycle Bin empty.png"),
		renderer->createTexture("icons/Recycle Bin.png"),
		renderer->createTexture("icons/Tablet PC.png"),
		renderer->createTexture("icons/Winamp.png"),
		renderer->createTexture("icons/Windows Media Player 10.png"),
		//renderer->createTexture("icons/.png")
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
		if (icon->downCollision(object.x, object.y + 1, object.w, object.h))
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


CollisionSide Map::collides(int x, int y, int w, int h, SDL_Rect* collider) {
	for (Icon* icon : icons) {
		if (icon->collides(x, y, w, h)) {
			icon->fetchCollisionBox(collider);
			
			float cx = x + w * 0.5;
			float cy = y + h * 0.5;
			float hx = (h + icon->getSize()) * (cx - icon->getCenterX());
			float wy = (w + icon->getSize()) * (cy - icon->getCenterY());
			
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