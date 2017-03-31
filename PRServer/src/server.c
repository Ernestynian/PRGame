#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

#include "../../Common/networkInterface.h"
#include "../../Common/byteConverter.h"

#include "server.h"
#include "map.h"
#include "client.h"
#include "players.h"


int srv_fd = 0;
struct timeval lastSentPacketTime;

char            inBuffer[MAX_CLIENT_PACKET_SIZE];
char            outBuffer[MAX_SERVER_PACKET_SIZE];
int             outBufferPosition;
unsigned char   outBufferTick;
pthread_mutex_t outBufferMutex = PTHREAD_MUTEX_INITIALIZER;
double          msTimeToSendPacket = 1000.0 / FRAMERATE;

MapData*        map;


int srv_start() {
	map = map_create();
	
	srv_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (srv_fd < 0) {
		printf("ERROR: Cannot create socket");
		return 0;
	}
	
	Address_in srv_address;
	memset(&srv_address, 0, sizeof(srv_address));
	srv_address.sin_family      = AF_INET;
	srv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_address.sin_port        = htons(PORT);

	if (bind(srv_fd, (Address*)&srv_address, sizeof(srv_address)) < 0) {
		printf("ERROR: Bind failed");
		return 0;
	}
	
	unsigned char* ip = (unsigned char*)&srv_address.sin_addr.s_addr;
	printf("Binded the server to %hhu.%hhu.%hhu.%hhu:%d\n", 
			ip[0], ip[1], ip[2], ip[3], PORT);

	int nonBlocking = 1;
	if (fcntl(srv_fd, F_SETFL, O_NONBLOCK, nonBlocking) < 0) {
		printf("ERROR: Failed to set non-blocking\n");
		return 0;
	}

	outBufferTick = 1;
	
	memset(inBuffer, 0, sizeof(inBuffer));
	memset(outBuffer, 0, sizeof(outBuffer));
	outBufferPosition = 1;

	return 1;
}


void srv_stop() {
	map_free(map);
	close(srv_fd);
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

	sendto(srv_fd, buffer, buflen, 0, (Address*)&client_address, addrlen);
	
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
	
	sendto(srv_fd, buffer, len, 0, (Address*)&client_address, addrlen);
}


void srv_sendClientAcceptAndMapData(char newClientId, Address_in client_address, socklen_t addrlen) {
	int mapDataLength;
	int mapIconsCount;
	char* mapData = map_getInitData(map, &mapDataLength, &mapIconsCount);
	
	char* packetData = (char*)malloc(5 + mapDataLength);
	packetData[0] = 0;
	packetData[1] = NET_EVENT_CLIENT_ACCEPTED;
	packetData[2] = 2 + mapDataLength;
	packetData[3] = newClientId;
	packetData[4] = mapIconsCount;
	memcpy(packetData + 5, mapData, mapDataLength);
	
	free(mapData);
	
	sendto(srv_fd, packetData, mapDataLength + 5, 0, 
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
	
	pthread_mutex_lock(&outBufferMutex);
	if (outBufferPosition > 1) {
		outBuffer[0] = tick;
		//printf("Sent %d bytes to everyone\n", outBufferPosition);

		pthread_mutex_lock(&clientListMutex);
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			sendto(srv_fd, outBuffer, outBufferPosition, 0, 
					(Address*)&clients[i].address, 
					sizeof(clients[i].address));
		}
		pthread_mutex_unlock(&clientListMutex);

		// outBufferPosition must be reset before unlocking mutex
		outBufferPosition = 1;
		result = 1;
	} else
		result = 0;
	pthread_mutex_unlock(&outBufferMutex);
	
	return result;
}


int srv_transferPackets() {
	Address_in client_address;
	socklen_t addrlen = sizeof(client_address);
	
	int recvlen = recvfrom(srv_fd, inBuffer, MAX_CLIENT_PACKET_SIZE, 0, 
							(Address*)&client_address, &addrlen);
	
	if (recvlen > 0) {
		if (inBuffer[1] == NET_EVENT_CLIENT_JOIN) {
			MapData* mapClone = (MapData*)malloc(sizeof(MapData));
			memcpy(mapClone, map, sizeof(MapData));
			
			int newClientID = client_create(client_address, mapClone);
			if (newClientID != CLIENT_NOT_CREATED) {
				srv_sendClientAcceptAndMapData(newClientID, client_address, addrlen);
				srv_sendCurrentState(newClientID, client_address, addrlen);
				
				srv_addNewEvent(NET_EVENT_CLIENT_JOIN, "1", (char)newClientID);
				
				printf("EVENT_CLIENT_JOIN assigned to: %d\n", newClientID);
			} else
				printf("EVENT_CLIENT_JOIN not accepted\n");
		} else {
			client_transferPacket(client_address, inBuffer, recvlen);
		}
		
		return 1;
	}
	
	
	struct timeval current;
	gettimeofday(&current, NULL);
	double 
	diff =  (current.tv_sec - lastSentPacketTime.tv_sec) * 1000.0;
	diff += (current.tv_usec - lastSentPacketTime.tv_usec) / 1000.0;
	if (diff > msTimeToSendPacket) {
		if (srv_sendEventsToAll(outBufferTick)) {
			gettimeofday(&lastSentPacketTime, NULL);
			outBufferTick++;
			if (outBufferTick == 0)
				outBufferTick++;
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
	
	pthread_mutex_lock(&outBufferMutex);	
	if (outBufferPosition + event->length < MAX_SERVER_PACKET_SIZE) {
		memcpy(outBuffer + outBufferPosition, event->data, event->length);
		outBufferPosition += event->length;
	}
	pthread_mutex_unlock(&outBufferMutex);	
	
	releaseEvent(event);
	free(bytes);
	
	va_end(args);
}