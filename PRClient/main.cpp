#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
	SDL_Window* window(0);
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Quit();
		
		return -1;
	}
	
	window = SDL_CreateWindow("SDL 2.0 Test",
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				800, 600, SDL_WINDOW_SHOWN);
	
	SDL_Delay(3000);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}