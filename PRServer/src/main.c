#include <stdio.h>
#include <unistd.h>

#include "server.h"

int main(int argc, const char* argv[]) {
	if (!srv_start())
		return 1;
	
	for (;;) {
		while(srv_getPacket());

		usleep(1);
	}
	
	printf("Press enter to exit.");
	getchar();
	
	srv_release();
	return 0;
}