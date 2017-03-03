#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "Video.h"
#include "Network.h"
#include "World.h"

class Game {
public:
	Game(int argc, const char* argv[]);
	~Game();
	
	int run();

private:
	void makeConnection();
	void processEvents();
	
	bool running;
	Video* video;
	World* world;
	
	const unsigned int msPerFrame;
	const unsigned int networkTickrate;
	
	Network* network;
	unsigned char frame;
};

#endif /* GAME_H */

