#ifndef CLIENT_H
#define CLIENT_H

#define CLIENT_NOT_CREATED MAX_CLIENTS

typedef struct {
	const pthread_t thread;	
	const unsigned char id;
	
	pthread_mutex_t mutex;
	char packetData[MAX_CLIENT_PACKET_SIZE];
	int  packetLength;
	char packetStatus;
} client_publicData;


typedef struct {
	client_publicData* cd;
	struct sockaddr_in address;
} client;

extern client clients[MAX_CLIENTS];
extern pthread_mutex_t clientListMutex;

//extern unsigned int currentClients;
//extern pthread_mutex_t clientsMutex;

uint32_t client_create(struct sockaddr_in client_address, MapData* mapClone);
void client_transferPacket(struct sockaddr_in client_address, char* data, int dataLen);

#endif /* CLIENT_H */

