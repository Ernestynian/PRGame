#include "Sprite.h"

Sprite::Sprite(Texture* texture, int x, int y) {
	this->texture = texture;
	renderQuad.x = x;
	renderQuad.y = y;
	renderQuad.w = texture->getWidth();
	renderQuad.h = texture->getHeight();
}

Sprite::Sprite(const Sprite& orig) { }

Sprite::~Sprite() { }


void Sprite::draw() {
	texture->draw(NULL, &renderQuad);
}
