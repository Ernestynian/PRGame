#include <SDL2/SDL.h>

#include "Texture.h"

#ifndef SPRITE_H
#define SPRITE_H

class Sprite {
public:
	Sprite(Texture* texture, int x, int y);
	Sprite(Texture* texture, int x, int y, int w, int h);
	virtual ~Sprite();

	virtual void draw();
	
protected:
	Texture* texture;
	SDL_Rect renderQuad;
};

#endif /* SPRITE_H */

