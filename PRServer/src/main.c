#include <stdio.h>
#include <unistd.h>

#include "server.h"
#include "players.h"


int main(int argc, const char* argv[]) {
	if (!srv_start())
		return 1;
	
	players_init();
	
	for (;;) {
		while(srv_transferPackets());

		usleep(10);
	}
	
	printf("Press enter to exit.");
	getchar();
	
	srv_stop();
	return 0;
}