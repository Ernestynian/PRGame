#ifndef ICON_H
#define ICON_H

#include "Texture.h"
#include "Sprite.h"

class Icon : public Sprite {
public:
	Icon(Texture* texture, int x, int y, int size);
	virtual ~Icon();
	
	bool hcollides(int x, int w);
	bool vcollides(int y, int h);
	bool collides(int x, int y, int w, int h);
	bool downCollision(int x, int y, int w, int h);
	
private:
	SDL_Rect collisionBox;
};

#endif /* ICON_H */

