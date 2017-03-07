#ifndef MAP_H
#define MAP_H

#include "Renderer.h"
#include "Sprite.h"

class Map {
public:
	Map(Renderer* renderer);
	virtual ~Map();
	
	bool canFall(SDL_Rect object);
	bool collides(SDL_Rect* object);
	
	void draw();
private:
	Sprite* background;
	SDL_Rect playerSpace;
};

#endif /* MAP_H */

