#ifndef RENDERER_H
#define RENDERER_H

#include <list>

class Renderer {
public:	
	Renderer(SDL_Window* window);
	~Renderer();
	
	void clear();
	void render();
	
	SDL_Texture* createTextureFromSurface(SDL_Surface* surface);
	
        void draw(SDL_Texture* texture, SDL_Rect* sourceQuad, SDL_Rect* renderQuad);
	
private:
	SDL_Renderer* renderer;

	std::list<SDL_Texture*> textures;
};

#endif /* RENDERER_H */

