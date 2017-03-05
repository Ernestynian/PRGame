#ifndef WORLD_H
#define WORLD_H

#include "Renderer.h"
#include "Sprite.h"

class World {
public:
	World(Renderer* renderer);
	~World();
	
	void update();
	void draw();
	
private:
	Renderer* renderer;
	
	Sprite* background;
};

#endif /* WORLD_H */

