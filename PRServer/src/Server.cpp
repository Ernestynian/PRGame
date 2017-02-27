#include <cstdlib>
#include <cstdio>

#include "Server.h"

Server::Server(int argc, const char* argv[]) { }

Server::~Server() { }

int Server::start() {
	printf("Terminal test. Press enter to exit.");
	
	getchar();
	
	return 0;
}