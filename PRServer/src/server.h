#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <sys/time.h>

#include "../../Common/networkInterface.h"

#include "map.h"


typedef struct sockaddr Address;
typedef struct sockaddr_in Address_in;

typedef struct {
	int             socket;
	struct timeval  lastSentPacketTime;

	char            inBuffer[MAX_CLIENT_PACKET_SIZE];
	char            outBuffer[MAX_SERVER_PACKET_SIZE];
	int             outBufferPosition;
	unsigned char   outBufferTick;
	pthread_mutex_t outBufferMutex;
	const double    msTimeToSendPacket;

	MapData*        map;
} Server;

int  srv_start();
void srv_stop();
int  srv_transferPackets();
void srv_addNewEvent(char eventType, const char* format, ...);

#endif /* SERVER_H */