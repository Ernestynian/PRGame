#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "../../Common/networkInterface.h"
#include "../../Common/byteConverter.h"

#include "server.h"
#include "client.h"
#include "players.h"


Server srv;

int srv_start() {
	srv.socket = 0;
	pthread_mutex_init(&srv.outBufferMutex, NULL);
	*(double*)&srv.msTimeToSendPacket = 1000.0 / FRAMERATE;
	
	srv.outBufferTick = 1;
	
	memset(srv.inBuffer, 0, sizeof(srv.inBuffer));
	memset(srv.outBuffer, 0, sizeof(srv.outBuffer));
	srv.outBufferPosition = 1;
	
	srv.map = map_create();
	
	// Initialize network connection
	
	srv.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (srv.socket < 0) {
		printf("ERROR: Cannot create socket");
		return 0;
	}
	
	Address_in srv_address;
	memset(&srv_address, 0, sizeof(srv_address));
	srv_address.sin_family      = AF_INET;
	srv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_address.sin_port        = htons(PORT);

	if (bind(srv.socket, (Address*)&srv_address, sizeof(srv_address)) < 0) {
		printf("ERROR: Bind failed");
		return 0;
	}
	
	unsigned char* ip = (unsigned char*)&srv_address.sin_addr.s_addr;
	printf("Binded the server to %hhu.%hhu.%hhu.%hhu:%d\n", 
			ip[0], ip[1], ip[2], ip[3], PORT);

	int nonBlocking = 1;
	if (fcntl(srv.socket, F_SETFL, O_NONBLOCK, nonBlocking) < 0) {
		printf("ERROR: Failed to set non-blocking\n");
		return 0;
	}
	
	return 1;
}


void srv_stop() {
	map_free(srv.map);
	close(srv.socket);
}


void srv_send(char eventType, Address_in client_address, socklen_t addrlen, const char* format, ...) {
	va_list args;
	va_start(args, format);
	
	int size;
	char* bytes = toBytes(&size, format, args);
	NetworkEvent* event = createEvent(eventType, bytes, size);

	int buflen = 1 + event->length;
	char* buffer = (char*)malloc(buflen);

	buffer[0] = 0;
	memcpy(buffer + 1, event->data, event->length);
	
	releaseEvent(event);

	sendto(srv.socket, buffer, buflen, 0, (Address*)&client_address, addrlen);
	
	free(buffer);
	
	va_end(args);
}


void srv_addNewEventTo(char* buffer, int* pos, char eventType, const char* format, ...) {
	va_list args;
    va_start(args, format);
	
	int size;
	char* bytes = toBytes(&size, format, args);
	NetworkEvent* event = createEvent(eventType, bytes, size);
	
	if (*pos + event->length < MAX_SERVER_PACKET_SIZE) {
		memcpy(buffer + *pos, event->data, event->length);
		*pos += event->length;
	}
	
	releaseEvent(event);
	free(bytes);
	
	va_end(args);
}


void srv_sendCurrentState(char newClientID, Address_in client_address, socklen_t addrlen) {
	int len = 1;
	char buffer[MAX_SERVER_PACKET_SIZE];
	buffer[0] = 0;
	
	pthread_mutex_lock(&clientListMutex);
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (newClientID != i && clients[i].pd != NULL) {
			srv_addNewEventTo(buffer, &len, NET_EVENT_CLIENT_JOIN, 
					"1", (char)clients[i].pd->id);
			
			if (player_isAlive(i)) {
				float x, y;
				player_getPos(i, &x, &y);
				srv_addNewEventTo(buffer, &len, NET_EVENT_PLAYER_SPAWN, 
					"1ff", (char)clients[i].pd->id, x, y);
			}
		}
	}
	pthread_mutex_unlock(&clientListMutex);
	
	sendto(srv.socket, buffer, len, 0, (Address*)&client_address, addrlen);
}


void srv_sendClientAcceptAndMapData(char newClientId, Address_in client_address, socklen_t addrlen) {
	int mapDataLength;
	int mapIconsCount;
	char* mapData = map_getInitData(srv.map, &mapDataLength, &mapIconsCount);
	
	char* packetData = (char*)malloc(5 + mapDataLength);
	packetData[0] = 0;
	packetData[1] = NET_EVENT_CLIENT_ACCEPTED;
	packetData[2] = 2 + mapDataLength;
	packetData[3] = newClientId;
	packetData[4] = mapIconsCount;
	memcpy(packetData + 5, mapData, mapDataLength);
	
	free(mapData);
	
	sendto(srv.socket, packetData, mapDataLength + 5, 0, 
			(Address*)&client_address, addrlen);
	
	free(packetData);	
}


/**
 * Send common accumulated events to everyone
 * @param tick - packet id
 * @return false when no events were send
 */
int srv_sendEventsToAll(unsigned char tick) {
	int result;
	
	pthread_mutex_lock(&srv.outBufferMutex);
	if (srv.outBufferPosition > 1) {
		srv.outBuffer[0] = tick;
		//printf("Sent %d bytes to everyone\n", outBufferPosition);

		pthread_mutex_lock(&clientListMutex);
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			sendto(srv.socket, srv.outBuffer, srv.outBufferPosition, 0, 
					(Address*)&clients[i].address, 
					sizeof(clients[i].address));
		}
		pthread_mutex_unlock(&clientListMutex);

		// outBufferPosition must be reset before unlocking mutex
		srv.outBufferPosition = 1;
		result = 1;
	} else
		result = 0;
	pthread_mutex_unlock(&srv.outBufferMutex);
	
	return result;
}


int srv_transferPackets() {
	Address_in client_address;
	socklen_t addrlen = sizeof(client_address);
	
	int recvlen = recvfrom(srv.socket, srv.inBuffer, MAX_CLIENT_PACKET_SIZE, 0, 
							(Address*)&client_address, &addrlen);
	
	if (recvlen > 0) {
		if (srv.inBuffer[1] == NET_EVENT_CLIENT_JOIN) {
			MapData* mapClone = (MapData*)malloc(sizeof(MapData));
			memcpy(mapClone, srv.map, sizeof(MapData));
			
			int newClientID = client_create(client_address, mapClone);
			if (newClientID != CLIENT_NOT_CREATED) {
				srv_sendClientAcceptAndMapData(newClientID, client_address, addrlen);
				srv_sendCurrentState(newClientID, client_address, addrlen);
				
				srv_addNewEvent(NET_EVENT_CLIENT_JOIN, "1", (char)newClientID);
				
				printf("EVENT_CLIENT_JOIN assigned to: %d\n", newClientID);
			} else
				printf("EVENT_CLIENT_JOIN not accepted\n");
		} else {
			client_transferPacket(client_address, srv.inBuffer, recvlen);
		}
		
		return 1;
	}
	
	
	struct timeval current;
	gettimeofday(&current, NULL);
	double 
	diff =  (current.tv_sec - srv.lastSentPacketTime.tv_sec) * 1000.0;
	diff += (current.tv_usec - srv.lastSentPacketTime.tv_usec) / 1000.0;
	if (diff > srv.msTimeToSendPacket) {
		if (srv_sendEventsToAll(srv.outBufferTick)) {
			gettimeofday(&srv.lastSentPacketTime, NULL);
			srv.outBufferTick++;
			if (srv.outBufferTick == 0)
				srv.outBufferTick++;
		}
	}
	
	return 0;
}


void srv_addNewEvent(char eventType, const char* format, ...) {
	va_list args;
    va_start(args, format);
	
	int size;
	char* bytes = toBytes(&size, format, args);
	NetworkEvent* event = createEvent(eventType, bytes, size);
	
	pthread_mutex_lock(&srv.outBufferMutex);	
	if (srv.outBufferPosition + event->length < MAX_SERVER_PACKET_SIZE) {
		memcpy(srv.outBuffer + srv.outBufferPosition, event->data, event->length);
		srv.outBufferPosition += event->length;
	}
	pthread_mutex_unlock(&srv.outBufferMutex);	
	
	releaseEvent(event);
	free(bytes);
	
	va_end(args);
}