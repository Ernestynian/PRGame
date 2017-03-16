#include <SDL2/SDL.h>

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
        SDL_Rect boundaries = {(int) x, (int) y, w, h};
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

bool Player::attack() {
    //waits till the end of previous attack, and allows attacks only while moving
    //TODO: jump attack
    if (deltaAttackTime <= 0.01) {
        deltaAttackTime = 0.99;

        if (x_speed == 0) {
            handsAnimationID = STILL_AND_ATTACKING;
        } else {
            handsAnimationID = RUNNING_AND_ATTACKING;
        }
        return true;
    } else {
        return false;
    }
    if (deltaAttackTime <= 0.01 && x_speed != 0) {
        deltaAttackTime = 0.99;
        return true;
    } else {

    }
}

void Player::move(Map* map, float delta) {
    if (state == PLAYER_STILL
            && x_speed == 0.0 && y_speed == 0.0)
        return;

    int x = (int) (this->x + x_speed * delta);
    int y = (int) (this->y + y_speed * delta);

    if (state != PLAYER_STILL) {
        CollisionSide collisionSide = map->collides(x, y, w, h);
        if (collisionSide != NotCollided) {
            for (int i = this->x; i < x; ++i)
                if (!map->collides(i, this->y, w, h))
                    this->x = i;

            for (int i = this->y; i < y; ++i)
                if (!map->collides(this->x, i, w, h))
                    this->y = i;

            if (collisionSide == CollidedBottom)
                x_speed *= 0.6;
            else
                x_speed = 0;
            y_speed = 0;

            SDL_Rect boundaries = {(int) this->x, (int) this->y, w, h};
            if (map->canFall(boundaries))
                changeStateTo(PLAYER_FALLING);
            else
                changeStateTo(PLAYER_STILL);

            return;
        }
    }

    if (!map->hcollides(&x, w)) {
        this->x += x_speed * delta;
    } else {
        x_speed = 0;
        y_speed = 0;
        if (state == PLAYER_MOVING) {
            this->x = x;
            changeStateTo(PLAYER_STILL);
        }
    }

    if (!map->vcollides(&y, h)) {
        this->y += y_speed * delta;
    } else if (state == PLAYER_FALLING) {
        this->y = y;
        changeStateTo(PLAYER_STILL);
    }

    /*switch(state) {
    case PLAYER_STILL:
        printf("PLAYER_STILL\n");
        break;
    case PLAYER_FALLING:
        printf("PLAYER_FALLING\n");
        break;
    case PLAYER_MOVING:
        printf("PLAYER_MOVING\n");
        break;
    case PLAYER_JUMPING:
        printf("PLAYER_JUMPING\n");
        break;
    case PLAYER_DYING:
        printf("PLAYER_DYING\n");
        break;
    case PLAYER_CROUCHING:
        printf("PLAYER_CROUCHING\n");
        break;
    }*/
}

bool Player::hasMoved() {
    return state == PLAYER_MOVING
            || state == PLAYER_FALLING
            || state == PLAYER_JUMPING;
}

void Player::calculateAnimation(float delta) {
    if (x_speed == 0.0) {
        deltaAnimTime = 0;
        //flip = SDL_FLIP_NONE;
    } else {
        deltaAnimTime += fabs(x_speed * 0.01 * delta);
        if (deltaAnimTime > animCycleTime)
            deltaAnimTime = deltaAnimTime - animCycleTime;
        if (x_speed > 0)
            flip = SDL_FLIP_NONE;
        else
            flip = SDL_FLIP_HORIZONTAL;
    }

    bodyAnimFrame = static_cast<int> (((float) animFrameCount / animCycleTime) * deltaAnimTime);

    if (deltaAttackTime > 0.0) {
        handsAnimFrame = 3 - floor(deltaAttackTime * attackAnimFrameCount);
        deltaAttackTime -= 0.005 * delta; //const to be tweaked
    } else {
        if (handsAnimationID != RUNNING) {
            handsAnimationID = RUNNING;
        }
        handsAnimFrame = static_cast<int> (((float) animFrameCount / animCycleTime) * deltaAnimTime); //temp
    }
}

void Player::draw() {
    SDL_Rect renderQuad = {(int) x, (int) y, w, h};
    SDL_Rect bodySourceQuad = {0 + bodyAnimFrame * tileW, 0, tileW, tileH};
    SDL_Rect handsSourceQuad = {0 + handsAnimFrame * tileW, handsAnimationID * tileH, tileW, tileH}; //to be tested

    bodyTexture->draw(&bodySourceQuad, &renderQuad, 0, NULL, flip);
    handsTexture->draw(&handsSourceQuad, &renderQuad, 0, NULL, flip); //
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