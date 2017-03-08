#ifndef RENDERER_H
#define RENDERER_H

#include <list>

class Renderer {
public:	
	Renderer(SDL_Window* window, int width, int height);
	~Renderer();
	
	void clear();
	void render();
	
	SDL_Texture* createTextureFromSurface(SDL_Surface* surface);
	
        void draw(SDL_Texture* texture, SDL_Rect* sourceQuad, SDL_Rect* renderQuad);
        void draw(SDL_Texture* texture, SDL_Rect* clipQuad, SDL_Rect* renderQuad, double angle, SDL_Point* center, SDL_RendererFlip flip);
	
	const int width;
	const int height;
	
private:
	SDL_Renderer* renderer;

	std::list<SDL_Texture*> textures;
};

#endif /* RENDERER_H */

