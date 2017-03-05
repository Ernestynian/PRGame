#include <SDL2/SDL.h>

#include "Texture.h"
#include "Player.h"


Player::Player(Texture* texture) {
	this->texture = texture;
	x = 0;
	y = 0;
}


Player::~Player() {

}


void Player::draw() {
	SDL_Rect renderQuad = { x, y, 32, 32 };
	
	texture->draw(&renderQuad);
}
