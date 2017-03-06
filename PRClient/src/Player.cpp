#include <SDL2/SDL.h>

#include "Texture.h"
#include "Player.h"


Player::Player(Texture* texture) {
	this->texture = texture;
	x = 0;
	y = 0;
	
	alive = false;
}


Player::~Player() {

}


void Player::spawn(int x, int y) {
	if (!isAlive()) {
		alive = true;
		this->x = x;
		this->y = y;
	}
}


void Player::draw() {
	SDL_Rect renderQuad = { x, y, 32, 32 };
	
	texture->draw(&renderQuad);
}


bool Player::isAlive() {
	return alive;
}