#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

class Game {
public:
	Game();
	int run(int argc, char* argv[]);
	~Game();
	
private:
	SDL_Window* window;
};

#endif /* GAME_H */

