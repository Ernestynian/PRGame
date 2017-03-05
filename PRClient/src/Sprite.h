#include <SDL2/SDL.h>

#include "Texture.h"

#ifndef SPRITE_H
#define SPRITE_H

class Sprite {
public:
	Sprite(Texture* texture, int x, int y);
	Sprite(const Sprite& orig);
	virtual ~Sprite();

	void draw();
	
private:
	Texture* texture;
	SDL_Rect renderQuad;
};

#endif /* SPRITE_H */

