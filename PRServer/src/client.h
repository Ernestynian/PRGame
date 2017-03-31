#ifndef CLIENT_H
#define CLIENT_H

#define CLIENT_NOT_CREATED MAX_CLIENTS


typedef struct Packet Packet;
struct Packet {
	const char * const data;
	const int length;
	
	Packet* next;
};

typedef struct {
	const pthread_t thread;	
	const unsigned char id;
	
	Packet* packets;
	pthread_mutex_t packets_mutex;
} client_PublicData;


typedef struct {
	client_PublicData* pd;
	struct sockaddr_in address;
} Client;

extern Client clients[MAX_CLIENTS];
extern pthread_mutex_t clientListMutex;

uint32_t client_create(struct sockaddr_in client_address, MapData* mapClone);
void client_transferPacket(struct sockaddr_in client_address, char* data, int dataLen);

#endif /* CLIENT_H */

