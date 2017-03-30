#include <limits>

#include <SDL2/SDL.h>

#include "../../Common/networkInterface.h"

#include "Texture.h"
#include "Player.h"


Player::Player(Texture* bodyTexture, Texture* handsTexture)
: tileW(320), tileH(320), animCycleTime(0.4), animFrameCount(4),
attackAnimFrameCount(4), attackAnimTime(0.4), deltaAttackTime(0.0),
flip(SDL_FLIP_NONE), handsAnimationID(RUNNING) {
	this->bodyTexture = bodyTexture;
	this->handsTexture = handsTexture;

	// TODO: more precise collision box
	x = 0;
	y = 0;
	w = tileW / 8;
	h = tileH / 8;

	x_speed = 0;
	y_speed = 0;

	alive = false;

	state = PLAYER_STILL;
}


Player::~Player() { }


void Player::spawn(int x, int y) {
	if(!isAlive()) {
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
	this->x_speed = 0;
	this->y_speed = 0;
	this->state = PLAYER_STILL;
}


void Player::setSpeed(float vx, float vy) {
	x_speed = vx;
	y_speed = vy;
}


void Player::applyGravity(Map* map, float g) {
	SDL_Rect boundaries = getCollisionBox();
	if(map->canFall(boundaries)) {
		y_speed += g;

		if(y_speed > 0.0)
			changeStateTo(PLAYER_FALLING);
	}
}


void Player::addSpeed(float x) {
	if(canMove()) {
		if(x_speed > 0
		&& x_speed + x > PLAYER_MAX_MOVEMENT_SPEED)
			x_speed = PLAYER_MAX_MOVEMENT_SPEED;
		else if(x_speed < 0
		&& x_speed + x < -PLAYER_MAX_MOVEMENT_SPEED)
			x_speed = -PLAYER_MAX_MOVEMENT_SPEED;
		else
			x_speed += x;

		changeStateTo(PLAYER_MOVING);
	}
}


void Player::applyFriction(float x) {
	if(state == PLAYER_MOVING) {
		if(x_speed < 0) {
			if(x_speed - x > 0)
				changeStateTo(PLAYER_STILL);
			else
				x_speed += x;
		} else if(x_speed > 0) {
			if(x_speed - x < 0)
				changeStateTo(PLAYER_STILL);
			else
				x_speed -= x;
		}
	}
}


bool Player::tryToJump(float speed) {
	if(canMove()) {
		y_speed = speed;
		changeStateTo(PLAYER_JUMPING);
		return true;
	}

	return false;
}


bool Player::attack() {
	//waits till the end of previous attack, and allows attacks only while moving
	//TODO: jump attack
	if(deltaAttackTime <= 0.01) {
		deltaAttackTime = 0.99;

		if(x_speed == 0) {
			handsAnimationID = STILL_AND_ATTACKING;
		} else {
			handsAnimationID = RUNNING_AND_ATTACKING;
		}
		return true;
	} else {
		return false;
	}
	if(deltaAttackTime <= 0.01 && x_speed != 0) {
		deltaAttackTime = 0.99;
		return true;
	} else {

	}
}


void Player::move(Map* map, float delta) {
	if(state == PLAYER_STILL
	&& x_speed == 0.0 && y_speed == 0.0)
		return;

	SDL_Rect boundaries = getCollisionBox();
	SDL_Rect newBoundaries = getCollisionBox(x_speed * delta, y_speed * delta);

	if(state != PLAYER_STILL) {
		SDL_Rect collider;
		CollisionSide collisionSide = map->collides(newBoundaries.x, newBoundaries.y, boundaries.w, boundaries.h, &collider);
		if(collisionSide != NotCollided) {
			int x_offset = newBoundaries.x - boundaries.x;
			int y_offset = newBoundaries.y - boundaries.y;
			
			if (x_offset != 0 || y_offset != 0) {
				float xInvEntry, yInvEntry;

				if(x_offset > 0.0f)
					xInvEntry = collider.x - (boundaries.x + boundaries.w);
				else
					xInvEntry = (collider.x + collider.w) - boundaries.x;

				if(y_offset > 0.0f) 
					yInvEntry = collider.y - (boundaries.y + boundaries.h);
				else
					yInvEntry = (collider.y + collider.h) - boundaries.y;

				float xEntry = -std::numeric_limits<float>::infinity();
				float yEntry = -std::numeric_limits<float>::infinity();

				if (x_offset != 0)
					xEntry = xInvEntry / x_offset;
				if (y_offset != 0)
					yEntry = yInvEntry / y_offset;

				float entryTime = std::max(xEntry, yEntry);

				this->x += x_offset * entryTime;
				boundaries.x += x_offset * entryTime;

				this->y += y_offset * entryTime;
				boundaries.y += y_offset * entryTime;
			}

			if(collisionSide == CollidedBottom)
				x_speed *= 0.75;
			else
				x_speed = 0;

			y_speed = 0;

			if(y_offset) {
				if(map->canFall(boundaries))
					changeStateTo(PLAYER_FALLING);
				else
					changeStateTo(PLAYER_STILL);
			}

			return;
		}
	}

	if(!map->hcollides(&newBoundaries.x, boundaries.w)) {
		this->x += x_speed * delta;
	} else if(state == PLAYER_MOVING) {
		this->x = newBoundaries.x - PLAYER_X_OFFSET;
		changeStateTo(PLAYER_STILL);
	}

	if(!map->vcollides(&newBoundaries.y, boundaries.h)) {
		this->y += y_speed * delta;
	} else if(state == PLAYER_FALLING) {
		this->y = newBoundaries.y - PLAYER_Y_OFFSET;
		changeStateTo(PLAYER_STILL);
	}
}


bool Player::hasMoved() {
	return state == PLAYER_MOVING
	|| state == PLAYER_FALLING
	|| state == PLAYER_JUMPING;
}


bool Player::hasStopped() { // TODO: change to onEvent design pattern
	if(stopped) {
		stopped = false;
		return true;
	}

	return false;
}


void Player::calculateAnimation(float delta) {
	if(x_speed == 0.0) {
		deltaAnimTime = 0;
		//flip = SDL_FLIP_NONE;
	} else {
		deltaAnimTime += fabs(x_speed * 0.01 * delta);
		if(deltaAnimTime > animCycleTime)
			deltaAnimTime = deltaAnimTime - animCycleTime;
		if(x_speed > 0)
			flip = SDL_FLIP_NONE;
		else
			flip = SDL_FLIP_HORIZONTAL;
	}

	bodyAnimFrame = static_cast<int>(((float)animFrameCount / animCycleTime) * deltaAnimTime);

	if(deltaAttackTime > 0.0) {
		handsAnimFrame = 3 - floor(deltaAttackTime * attackAnimFrameCount);
		deltaAttackTime -= 0.005 * delta; //const to be tweaked
	} else {
		if(handsAnimationID != RUNNING) {
			handsAnimationID = RUNNING;
		}
		handsAnimFrame = static_cast<int>(((float)animFrameCount / animCycleTime) * deltaAnimTime); //temp
	}
}


void Player::draw() {
	SDL_Rect renderQuad = {(int)x, (int)y, w, h};
	SDL_Rect bodySourceQuad = {0 + bodyAnimFrame * tileW, 0, tileW, tileH};
	SDL_Rect handsSourceQuad = {0 + handsAnimFrame * tileW, handsAnimationID * tileH, tileW, tileH}; //to be tested

	bodyTexture->draw(&bodySourceQuad, &renderQuad, 0, NULL, flip);
	handsTexture->draw(&handsSourceQuad, &renderQuad, 0, NULL, flip); //
}


bool Player::isAlive() {
	return alive;
}


bool Player::canMove() {
	if(!isAlive())
		return false;

	switch(state) {
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


void Player::printStatus() {
	switch(state) {
	case PLAYER_STILL:
		printf("PLAYER_STILL  ");
		break;
	case PLAYER_FALLING:
		printf("PLAYER_FALLING");
		break;
	case PLAYER_MOVING:
		printf("PLAYER_MOVING ");
		break;
	case PLAYER_JUMPING:
		printf("PLAYER_JUMPING");
		break;
	case PLAYER_DYING:
		printf("PLAYER_DYING  ");
		break;
	case PLAYER_CROUCHING:
		printf("PLAYER_CROUCHING");
		break;
	}
	
	printf(" %.5f %.5f\n", x_speed, y_speed);
}


SDL_Rect Player::getCollisionBox() {
	return getCollisionBox(0, 0);
}


SDL_Rect Player::getCollisionBox(float x_offset, float y_offset) {
	return SDL_Rect {
		(int)(x + x_offset) + PLAYER_X_OFFSET,
		(int)(y + y_offset) + PLAYER_Y_OFFSET,
		PLAYER_WIDTH,
		PLAYER_HEIGHT };
}


void Player::changeStateTo(PlayerState newState) {
	if(newState == PLAYER_STILL) {
		if (state != PLAYER_FALLING)
			x_speed = 0;
		else {
			x_speed *= 0.5;
			stopped = true;
		}
		
		y_speed = 0;
	}

	state = newState;
}