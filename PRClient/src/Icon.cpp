#include <SDL2/SDL.h>

#include "Icon.h"


Icon::Icon(Texture* texture, int x, int y, int size)
: Sprite(texture, x, y, size, size) {
	collisionBox.x = x;
	collisionBox.y = y;
	collisionBox.w = size;
	collisionBox.h = size;
}


Icon::~Icon() { }



bool Icon::hcollides(int x, int w) {
	if (x + w > collisionBox.x
	 && x     < collisionBox.x + collisionBox.w) {
		return true;
	}
	
	return false;
}


bool Icon::vcollides(int y, int h) {
	if (y + h > collisionBox.y
	 && y     < collisionBox.y + collisionBox.h) {
		return true;
	}
	
	return false;
}


bool Icon::collides(int x, int y, int w, int h) {
	return hcollides(x, w) && vcollides(y, h);
}


bool Icon::downCollision(int x, int y, int w, int h) {
	if (x + w > collisionBox.x
	 && x     < collisionBox.x + collisionBox.w
	 && y + h > collisionBox.y - 1
	 && y     < collisionBox.y + collisionBox.h)
		return true;
	
	return false;
}


float Icon::getCenterX() {
	return collisionBox.x + collisionBox.w * 0.5;
}


float Icon::getCenterY() {
	return collisionBox.y + collisionBox.h * 0.5;
}


void Icon::fetchCollisionBox(SDL_Rect* box) {
	box->x = collisionBox.x;
	box->y = collisionBox.y;
	box->w = collisionBox.w;
	box->h = collisionBox.h;
}