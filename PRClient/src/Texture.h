#ifndef TEXTURE_H
#define TEXTURE_H

#include "Renderer.h"

class Texture {
public:
	Texture(Renderer* renderer, const char* filename);
	virtual ~Texture();	
	
	int getWidth();
	int getHeight();
	
	void draw(SDL_Rect* sourceQuad, SDL_Rect* renderQuad);
	
private:
	SDL_Texture* texture;
	int width;
	int height;
	
	Renderer* renderer;
};

#endif /* TEXTURE_H */

