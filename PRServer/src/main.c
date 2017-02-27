#include <stdio.h>

#include "server.h"

int main(int argc, const char* argv[]) {
	srv_start();
	
	printf("Terminal test. Press enter to exit.");
	
	getchar();
}