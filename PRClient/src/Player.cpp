#include <limits>

#include <SDL2/SDL.h>

#include "../../Common/networkInterface.h"

#include "Texture.h"
#include "Player.h"


Player::Player(Texture* bodyTexture, Texture* handsTexture)
: tileW(320), tileH(320), /*animCycleTime(0.4), animFrameCount(4),*/
attackAnimFrameCount(4), attackAnimTime(0.4), deltaAttackTime(0.0),
flip(SDL_FLIP_NONE), handsAnimation(RUNNING), bodyAnimOffset(0) {
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
	sideHit = false;
	iconSideHitSpeed = 0;

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


void Player::kill(char direction) {
	alive = false;
	state = PLAYER_DYING;
	
	setSpeed(0.15 * direction, -0.2);
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
        
        //kind of a bandaid to a bug with connected clients not moving properly
        if(vx != 0 || vy != 0)
            changeStateTo(PLAYER_MOVING);
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
	if(state == PLAYER_MOVING || state == PLAYER_DYING) {
		if(x_speed < 0) {
			if(x_speed + x > 0)
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


bool Player::tryToJump(Map* map, float speed) {
	if(canMove()) {
		y_speed = speed;
		changeStateTo(PLAYER_JUMPING);
		return true;
	} else if (canBounce(map)) {
		if (x_speed == 0)
			x_speed = -iconSideHitSpeed;	
		else
			x_speed = -x_speed;
		
		iconSideHitSpeed = 0;
		
		y_speed = speed * 0.75;
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
			handsAnimation = STILL_AND_ATTACKING;
		} else {
			handsAnimation = RUNNING_AND_ATTACKING;
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
	if(state == PLAYER_STILL && x_speed ==  y_speed == 0.0)
		return;

	SDL_Rect boundaries = getCollisionBox();
	SDL_Rect newBoundaries = getCollisionBox(x_speed * delta, y_speed * delta);

	if(state != PLAYER_STILL) {
		SDL_Rect collider;
		CollisionSide collisionSide = map->collides(newBoundaries.x, newBoundaries.y,
										boundaries.w, boundaries.h, &collider);
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

			if (collisionSide == CollidedLeft
			 || collisionSide == CollidedRight) {
				iconSideHitSpeed = x_speed;
				iconSideHitHeight = collider.y + collider.h;
			} else
				iconSideHitSpeed = 0;
			
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

	// Collision with map borders (the window itself)
	
	if(!map->hcollides(&newBoundaries.x, boundaries.w)) {
		this->x += x_speed * delta;
		sideHit = false;
	} else {
		sideHit = true;
		if(state == PLAYER_MOVING) {
			this->x = newBoundaries.x - PLAYER_X_OFFSET;
			changeStateTo(PLAYER_STILL);
		}
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
    bodyAnimOffset = 0;//test it or change it
    switch(state)//can be optimised a little
    {
        case PLAYER_STILL:
            animFrameCount = 1;
            animCycleTime = 1;
            break;
        case PLAYER_MOVING:
            animFrameCount = 4;
            animCycleTime = 0.4;
            if(fabs(x_speed) > PLAYER_MAX_MOVEMENT_SPEED*0.90f)
            {
                bodyAnimOffset = 4;
            }
            else
            {
                bodyAnimOffset = 0;
            }
            if(deltaAttackTime <= 0.0) 
                handsAnimation = RUNNING;
            break;
        case PLAYER_FALLING:
            animFrameCount = 2;
            animCycleTime = 0.4;
            break;
        case PLAYER_JUMPING:
            animFrameCount = 4;
            animCycleTime = 0.6;
            break;
        case PLAYER_DYING:
            animFrameCount = 8;//4 of animation and 4 placeholders
            animCycleTime = 2;//to be corrected
            if(deltaAttackTime <= 0.0) 
                handsAnimation = DYING;
            break;
        default:
            break;
    }
    
    deltaAnimTime += fabs(x_speed * 0.01 * delta);
    if(deltaAnimTime > animCycleTime)
        deltaAnimTime = deltaAnimTime - animCycleTime;
    
    if(x_speed > 0)
        flip = SDL_FLIP_NONE;
    else
        flip = SDL_FLIP_HORIZONTAL;

	bodyAnimFrame = static_cast<int>(((float)animFrameCount / animCycleTime) * deltaAnimTime);

	if(deltaAttackTime > 0.0) {
		handsAnimFrame = 4 - floor(deltaAttackTime * attackAnimFrameCount);
		deltaAttackTime -= 0.005 * delta; //const to be tweaked
	} else {
            if( state == PLAYER_STILL ||
                state == PLAYER_JUMPING ||
                state == PLAYER_FALLING)
                handsAnimation = RUNNING;
                
		handsAnimFrame = static_cast<int>(((float)animFrameCount / animCycleTime) * deltaAnimTime); //temp
	}
}


void Player::draw() {
	SDL_Rect renderQuad = {(int)x, (int)y, w, h};
	SDL_Rect bodySourceQuad = {(bodyAnimOffset + bodyAnimFrame) * tileW, state * tileH, tileW, tileH}; //to be tested
	SDL_Rect handsSourceQuad = {(bodyAnimOffset + handsAnimFrame) * tileW, handsAnimation * tileH, tileW, tileH};

	bodyTexture->draw(&bodySourceQuad, &renderQuad, 0, NULL, flip);
	handsTexture->draw(&handsSourceQuad, &renderQuad, 0, NULL, flip); //
}


bool Player::isAlive() {
	return alive;
}


bool Player::isDying() {
	return state == PLAYER_DYING;
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


bool Player::canBounce(Map* map) {
	if (!isAlive())
		return false;
	
	switch(state) {
	case PLAYER_JUMPING:
	case PLAYER_FALLING: {
		if (sideHit)
			return true;
		
		if (iconSideHitSpeed != 0
		 && iconSideHitHeight > y + PLAYER_HEIGHT * 0.5)
			return true;
	}
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
	
	printf(" % .5f % .5f\n", x_speed, y_speed);
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
	if (newState == state)
		return;
	
	if (state == PLAYER_DYING && newState != PLAYER_STILL)
		return;
	
	if(newState == PLAYER_STILL) {
		y_speed = 0;
		
		if (state == PLAYER_FALLING) {
			x_speed *= 0.5;
			if (x_speed != 0.0) {
				changeStateTo(PLAYER_MOVING);
				return;
			}
		} else {
			x_speed = 0;
			stopped = true;
		}
	}

	if (state == PLAYER_FALLING)
		iconSideHitSpeed = 0;
	
	state = newState;
}