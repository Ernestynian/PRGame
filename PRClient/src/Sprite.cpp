#include "Sprite.h"


Sprite::Sprite(Texture* texture, int x, int y) {
	Sprite(texture, x, y, texture->getWidth(), texture->getHeight());
}


Sprite::Sprite(Texture* texture, int x, int y, int w, int h) {
	this->texture = texture;
	renderQuad.x = x;
	renderQuad.y = y;
	renderQuad.w = w;
	renderQuad.h = h;
}


Sprite::~Sprite() { }


void Sprite::draw() {
	texture->draw(NULL, &renderQuad);
}
