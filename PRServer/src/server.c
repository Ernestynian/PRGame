#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "server.h"
#include "client.h"

#include "../../Common/networkInterface.h"


int listenfd = 0;
struct sockaddr_in srv_address;


struct sockaddr_in client_address;
socklen_t addrlen = sizeof(client_address);
char buf[MAX_PACKET_SIZE];


int srv_start() {
	// Create IP/UDP socket
	if ((listenfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("ERROR: Cannot create socket");
		return 0;
	}
	
	memset(&srv_address, 0, sizeof(srv_address));
	srv_address.sin_family      = AF_INET;
	srv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_address.sin_port        = htons(PORT);

	if (bind(listenfd, (struct sockaddr*)&srv_address, sizeof(srv_address)) < 0) {
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


void srv_stop() {
	close(listenfd);	
}

	
int srv_checkForNewClients() {
	int recvlen = recvfrom(listenfd, buf, MAX_PACKET_SIZE, 0, (struct sockaddr*)&client_address, &addrlen);
	
	if (recvlen > 0) {
		if (buf[1] == EVENT_CLIENT_JOIN) {
			printf("EVENT_CLIENT_JOIN\n");
			client_create(client_address);
			// TODO: reply success if created
		} else {
			//printf("buf: %hhu\n", buf[0]);
			client_transferPacket(client_address, buf);
		}
		
		return 1;
	}
	
	return 0;
}

// sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen)
