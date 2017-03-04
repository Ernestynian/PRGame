#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "server.h"
#include "client.h"

#include "../../Common/networkInterface.h"
#include "players.h"


int srv_fd = 0;

char inbuffer[MAX_CLIENT_PACKET_SIZE];
char srv_buf[MAX_SERVER_PACKET_SIZE];


int srv_start() {
	// Create IP/UDP socket
	if ((srv_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("ERROR: Cannot create socket");
		return 0;
	}
	
	struct sockaddr_in srv_address;
	memset(&srv_address, 0, sizeof(srv_address));
	srv_address.sin_family      = AF_INET;
	srv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_address.sin_port        = htons(PORT);

	if (bind(srv_fd, (struct sockaddr*)&srv_address, sizeof(srv_address)) < 0) {
		printf("ERROR: Bind failed");
		return 0;
	}
	
	unsigned char* ip = (unsigned char*)&srv_address.sin_addr.s_addr;
	printf("Binded the server to %d.%d.%d.%d:%d\n", ip[0], ip[1], ip[2], ip[3], PORT);

	int nonBlocking = 1;
	if (fcntl(srv_fd, F_SETFL, O_NONBLOCK, nonBlocking) < 0) {
		printf("ERROR: Failed to set non-blocking\n");
		return 0;
	}

	memset(inbuffer, 0, sizeof(inbuffer));
	
	players_init();

	return 1;
}


void srv_stop() {
	close(srv_fd);
}


void srv_send(char eventType, struct sockaddr_in client_address, socklen_t addrlen) {
	NetworkEvent* event = createEvent(eventType, 0, 0);

	srv_buf[0] = 0;
	memcpy(srv_buf + 1, event->data, event->length);
	int buflen = 1 + event->length;

	releaseEvent(event);

	sendto(srv_fd, srv_buf, buflen, 0, (struct sockaddr *)&client_address, addrlen);
}


int srv_transferPackets() {
	struct sockaddr_in client_address;
	socklen_t addrlen = sizeof(client_address);
	
	int recvlen = recvfrom(srv_fd, inbuffer, MAX_CLIENT_PACKET_SIZE, 0, (struct sockaddr*)&client_address, &addrlen);
	
	if (recvlen > 0) {		
		if (inbuffer[1] == NET_EVENT_CLIENT_JOIN) {
			int newClientID = client_create(client_address);
			if (newClientID != CLIENT_NOT_CREATED) {
				srv_send(NET_EVENT_CLIENT_ACCEPTED, client_address, addrlen);
				player_reset(newClientID);
				printf("EVENT_CLIENT_JOIN not accepted\n");
			} else
				printf("EVENT_CLIENT_JOIN %d\n", newClientID);
		} else {
			client_transferPacket(client_address, inbuffer);
		}
		
		return 1;
	}
	
	return 0;
}


void srv_sendEventsToAll(unsigned char tick, unsigned char* eventsData, size_t dataLength) {
	srv_buf[0] = tick;
	memcpy(srv_buf + 1, eventsData, dataLength);
	int buflen = 1 + dataLength;
	
	send(srv_fd, srv_buf, buflen, 0);
}