#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

#include "Window.h"
#include "World.h"
#include "Network.h"

class Game {
public:
	Game(int argc, const char* argv[]);
	~Game();
	
	int run();

private:
	std::pair<int, std::vector<IconData*>> makeConnection();
	void processEvents();
	void checkPackets();
	void packetSendingThread();
        
	bool running;
	Window* window;
	World* world;
	
	const float msPerFrame;
	const unsigned int networkTickrate;
	
	Network* network;
};

#endif /* GAME_H */

