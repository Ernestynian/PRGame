#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "server.h"

#include "../../Common/networkInterface.h"
#include "../../Common/byteConverter.h"


int listenfd = 0;
struct sockaddr_in serv_addr;

char buf[PACKET_SIZE];

int srv_start() {
	// Create IP/UDP socket
	if ((listenfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("ERROR: Cannot create socket");
		return 0;
	}

	memset(&serv_addr, '0', sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
		perror("ERROR: Bind failed");
		return 0;
	}

	int nonBlocking = 1;
	if (fcntl(listenfd, F_SETFL, O_NONBLOCK, nonBlocking) < 0) {
		printf("ERROR: Failed to set non-blocking\n");
		return 0;
	}

	memset(buf, '0', sizeof(buf));

	return 1;
}


void srv_release() {
	close(listenfd);	
}


struct sockaddr_in client_addr;
socklen_t addrlen = sizeof(client_addr);
	
int srv_getPacket() {
	int recvlen = recvfrom(listenfd, buf, PACKET_SIZE, 0, (struct sockaddr*)&client_addr, &addrlen);
	
	if (recvlen > 0) {
		buf[recvlen] = 0;
		if (buf[1] > 0) {
			printf("received message: \"%s\" [%d %d]\n", buf + 2, buf[0], buf[1]);
			if (buf[0] == EVENT_PLAYER_MOVED) {
				initBinaryReader(buf + 2);
				int a = binaryRead4B();
				int b = binaryRead4B();
				printf("%d %d\n", a, b);
			}
		} else
			printf("received ping message: %d\n", buf[0]);
		return 1;
	}
	
	return 0;
}

// sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen)