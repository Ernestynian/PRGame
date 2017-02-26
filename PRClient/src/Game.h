#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "Video.h"

class Game {
public:
	Game(int argc, const char* argv[]);
	~Game();
	
	int run();

private:
	void processEvents();
	
	Video* video;
	bool running;
	
	unsigned int currentTime, lastTime;
	const unsigned int ticksPerFrame;
};

#endif /* GAME_H */

