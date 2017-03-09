#include <SDL2/SDL.h>

#include "Texture.h"
#include "Player.h"


Player::Player(Texture* texture) : tileW(320), tileH(480), 
        animCycleTime(0.4), anim_frame_count(4) {
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


void Player::teleportToPosition(int x, int y) {
	this->x = x;
	this->y = y;
}


void Player::applyGravity(Map* map, float g) {
	if (isAlive()) {
		SDL_Rect boundaries = { (int)x, (int)y, w, h };
		if (map->canFall(boundaries)) {
			y_speed += g;
			
			if (y_speed > 0.0)
				changeStateTo(PLAYER_FALLING);
		}			
	}
}


void Player::setSpeed(float x, float y) {
	if (canMove()) {
		x_speed = x;
		y_speed = y;
		changeStateTo(PLAYER_MOVING);
	}
}


bool Player::tryToJump(float speed) {
	if (canMove()) {
		y_speed = speed;
		changeStateTo(PLAYER_JUMPING);
		return true;
	}
	
	return false;
}


void Player::move(Map* map) {
	// TODO: collision when going up
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


bool Player::hasMoved() {
	return state == PLAYER_MOVING || state == PLAYER_FALLING || state == PLAYER_JUMPING;
}


void Player::calculateAnimation() {
    if(x_speed == 0)
    {
        deltaAnimTime = 0;
        flip = SDL_FLIP_NONE;
    }
    else
    {
        deltaAnimTime += fabs(x_speed*0.01);
        if(deltaAnimTime > animCycleTime)
            deltaAnimTime = deltaAnimTime - animCycleTime;
        if(x_speed >= 0)
            flip = SDL_FLIP_NONE;
        else
            flip = SDL_FLIP_HORIZONTAL;
    }
    
    anim_frame = static_cast<int>(((float)anim_frame_count / animCycleTime) * deltaAnimTime);
}
    
    
void Player::draw() {
	SDL_Rect renderQuad = { (int)x, (int)y, w, h };
	SDL_Rect bodySourceQuad = { 0 + anim_frame * tileW, 0, tileW, tileH };//temp
	SDL_Rect handsSourceQuad = { 0 + anim_frame * tileW, tileH, tileW, tileH };//temp
        
	texture->draw(&bodySourceQuad, &renderQuad, 0, NULL, flip);
    texture->draw(&handsSourceQuad, &renderQuad, 0, NULL, flip);//
}


bool Player::isAlive() {
	return alive;
}


bool Player::canMove() {
	if (!isAlive())
		return false;
	
	switch (state) {
	case PLAYER_STILL:
	case PLAYER_MOVING:
		return true;
	default:
		return false;
	}
}


float Player::getPosX() {
	return x;
}


float Player::getPosY() {
	return y;
}


float Player::getSpeedX() {
	return x_speed;
}


float Player::getSpeedY() {
	return y_speed;
}


void Player::changeStateTo(PlayerState newState) {
	if (newState == PLAYER_STILL) {
		x_speed = 0;
		y_speed = 0;
	}
	
	state = newState;
}