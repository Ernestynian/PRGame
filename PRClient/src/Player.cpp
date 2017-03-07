#include <SDL2/SDL.h>

#include "Texture.h"
#include "Player.h"


Player::Player(Texture* texture) : tileW(320), tileH(480) {
	this->texture = texture;
	x = 0;
	y = 0;
	w = tileW / 10;
	h = tileH / 10;
	
	x_speed = 0;
	y_speed = 0;
	
	alive = false;
	
	state = PLAYER_STILL;
}


Player::~Player() {

}


void Player::spawn(int x, int y) {
	if (!isAlive()) {
		alive = true;
		this->x = x;
		this->y = y;
		
		changeStateTo(PLAYER_STILL);
	}
}


void Player::kill() {
	alive = false;
}


void Player::moveToPosition(int x, int y) {
	this->x = x;
	this->y = y;
}


void Player::move(Map* map) {
	// TODO: check collisions with world
	SDL_Rect newBoundaries = { (int)(x + x_speed), (int)(y + y_speed), w, h };
	if (!map->collides(&newBoundaries)) {
		x += x_speed;
		y += y_speed;
	} else if (state == PLAYER_MOVING || state == PLAYER_FALLING) {
		x = newBoundaries.x;
		y = newBoundaries.y;
		changeStateTo(PLAYER_STILL);
	}
}


void Player::setSpeed(float x, float y) {
	x_speed = x;
	y_speed = y;
}


void Player::applyGravity(Map* map, float g) {
	SDL_Rect boundaries = { (int)x, (int)y, w, h };
	if (map->canFall(boundaries))
		changeStateTo(PLAYER_FALLING);
	
	if (state == PLAYER_FALLING)
		this->y_speed += g;
}


void Player::draw() {
	SDL_Rect renderQuad = { (int)x, (int)y, w, h };
	SDL_Rect sourceQuad = { 0, 0, tileW, tileH };//temp
	
	texture->draw(&sourceQuad, &renderQuad);
}


bool Player::isAlive() {
	return alive;
}


bool Player::canMove() {
	switch (state) {
	case PLAYER_STILL:
	case PLAYER_MOVING:
		return true;
	default:
		return false;
	}
}


void Player::changeStateTo(PlayerState newState) {
	if (newState == PLAYER_STILL) {
		x_speed = 0;
		y_speed = 0;
	}
	state = newState;
}