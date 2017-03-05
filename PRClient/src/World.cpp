#include <SDL2/SDL.h>

#include "Texture.h"
#include "World.h"


World::World(Renderer* renderer) {
	this->renderer = renderer;
	
	Texture* t = new Texture(renderer, "res/background.bmp");
	background = new Sprite(t, 0, 0);
}


World::~World() {
	delete background;
}


void World::update() {
	
}


void World::draw() {
	renderer->clear();
	
	background->draw();
	
	renderer->render();
}