#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.h"


Texture::Texture(Renderer* renderer, const char* filename) {
	this->renderer = renderer;
	
    int swidth, sheight, orig_format;
    unsigned char* data = stbi_load(filename, &swidth, &sheight, 
            &orig_format, STBI_rgb_alpha);
    if(data == NULL) {
      printf("Loading image failed: %s", stbi_failure_reason());
      //fatal error, do something
    }

    // Set up the pixel format color masks for RGBA byte arrays.
    // Only STBI_rgb and STBI_rgb_alpha are supported here!
    Uint32 rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      int shift = (req_format == STBI_rgb) ? 8 : 0;
      rmask = 0xff000000 >> shift;
      gmask = 0x00ff0000 >> shift;
      bmask = 0x0000ff00 >> shift;
      amask = 0x000000ff >> shift;
    #else // little endian
      rmask = 0x000000ff;
      gmask = 0x0000ff00;
      bmask = 0x00ff0000;
      amask = 0xff000000;
    #endif

    int depth, pitch;
    depth = 32;
    pitch = 4*swidth;

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)data, 
            swidth, sheight, depth, pitch, rmask, gmask, bmask, amask);
    texture = renderer->createTextureFromSurface(surface);
    SDL_FreeSurface(surface);
    stbi_image_free(data);//
	
	//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	
	SDL_QueryTexture(texture, 0, 0, &width, &height);
}


Texture::~Texture() {
	
}


int Texture::getWidth() {
	return width;
}


int Texture::getHeight() {
	return height;
}


void Texture::draw(SDL_Rect* sourceQuad, SDL_Rect* renderQuad) {
	renderer->draw(texture, sourceQuad, renderQuad);
}


void Texture::draw(SDL_Rect* sourceQuad, SDL_Rect* renderQuad, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	renderer->draw(texture, sourceQuad, renderQuad, angle, center, flip);
}