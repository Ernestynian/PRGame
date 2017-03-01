#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "Video.h"
#include "Network.h"

class Game {
public:
	Game(int argc, const char* argv[]);
	~Game();
	
	int run();

private:
	void processEvents();
	
	Video* video;
	bool running;
	
	const unsigned int msPerFrame;
	const unsigned int networkTickrate;
	
	Network network;
};

#endif /* GAME_H */

