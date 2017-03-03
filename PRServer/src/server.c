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


int srv_sock = 0;
struct sockaddr_in srv_address;


struct sockaddr_in client_address;
socklen_t addrlen = sizeof(client_address);
char buf[MAX_PACKET_SIZE];


int srv_start() {
	// Create IP/UDP socket
	if ((srv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("ERROR: Cannot create socket");
		return 0;
	}
	
	memset(&srv_address, 0, sizeof(srv_address));
	srv_address.sin_family      = AF_INET;
	srv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_address.sin_port        = htons(PORT);

	if (bind(srv_sock, (struct sockaddr*)&srv_address, sizeof(srv_address)) < 0) {
		perror("ERROR: Bind failed");
		return 0;
	}

	int nonBlocking = 1;
	if (fcntl(srv_sock, F_SETFL, O_NONBLOCK, nonBlocking) < 0) {
		printf("ERROR: Failed to set non-blocking\n");
		return 0;
	}

	memset(buf, '0', sizeof(buf));

	return 1;
}


void srv_stop() {
	close(srv_sock);	
}


void srv_send(char eventType) {
	NetworkEvent* event = createEvent(eventType, 0, 0);

	buf[0] = 0;
	memcpy(buf + 1, event->data, event->length);
	int buflen = 1 + event->length;

	releaseEvent(event);

	sendto(srv_sock, buf, buflen, 0, (struct sockaddr *)&client_address, addrlen);
}


int srv_transferPackets() {
	int recvlen = recvfrom(srv_sock, buf, MAX_PACKET_SIZE, 0, (struct sockaddr*)&client_address, &addrlen);
	
	if (recvlen > 0) {
		if (buf[1] == NET_EVENT_CLIENT_JOIN) {
			printf("EVENT_CLIENT_JOIN\n");
			int newClientID = client_create(client_address);
			if (newClientID != CLIENT_NOT_CREATED) {
				srv_send(NET_EVENT_CLIENT_ACCEPTED);
				player_reset(newClientID);
			}
		} else {
			//printf("buf: %hhu\n", buf[0]);
			client_transferPacket(client_address, buf);
		}
		
		return 1;
	}
	
	return 0;
}