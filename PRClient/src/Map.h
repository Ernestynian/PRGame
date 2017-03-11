#ifndef MAP_H
#define MAP_H

#include <vector>

#include "Renderer.h"
#include "Sprite.h"
#include "Icon.h"

class Map {
public:
	Map(Renderer* renderer);
	virtual ~Map();
	
	bool canFall(SDL_Rect object);
	bool hcollides(int* x, int w);
	bool vcollides(int* y, int h);
	bool collides(int x, int y, int w, int h);
	
	void draw();
private:
	Sprite* background;
	SDL_Rect playerSpace;
	
	std::vector<Icon*> icons;
};

#endif /* MAP_H */

