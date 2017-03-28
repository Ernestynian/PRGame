#ifndef MAP_H
#define MAP_H

#include <vector>

#include "Renderer.h"
#include "Sprite.h"
#include "Icon.h"

enum CollisionSide {
	NotCollided = 0,
	CollidedBottom = 1,
	CollidedTop,
	CollidedLeft,
	CollidedRight
};

struct IconData {
	int x;
	int y;
	int textureId;
};

class Map {
public:
	Map(Renderer* renderer, std::vector<IconData*> newIcons);
	virtual ~Map();
	
	bool canFall(SDL_Rect object);
	bool hcollides(int* x, int w);
	bool vcollides(int* y, int h);
	CollisionSide collides(int x, int y, int w, int h, SDL_Rect* collider);
	
	void draw();
	void drawObjects();
private:
	Sprite* background;
	SDL_Rect playerSpace;
	
	std::vector<Icon*> icons;
};

#endif /* MAP_H */

