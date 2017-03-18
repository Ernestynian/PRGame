#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "timer.h"

#include "../../Common/networkInterface.h"
#include "../../Common/byteConverter.h"

#include "server.h"
#include "client.h"
#include "map.h"
#include "players.h"


typedef struct {
	unsigned char currentTick;
	unsigned char previousTick;
	
	MapData* map;
	
	struct timeval lastPacketTime;
	int spawningPlayer;
	struct timeval spawnTimerStart;
} client_privateData;


client clients[MAX_CLIENTS];
pthread_mutex_t clientListMutex = PTHREAD_MUTEX_INITIALIZER;

//unsigned int currentClients = 0;
//pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;


void* client_stop(client_publicData* data, client_privateData* priv) {
	srv_addNewEvent(NET_EVENT_CLIENT_EXIT, "1", data->id);
	
	//pthread_mutex_lock(&clientsMutex);
	//currentClients--;
	//pthread_mutex_unlock(&clientsMutex);
	
	pthread_mutex_lock(&clientListMutex);
	clients[data->id].cd = NULL;
	pthread_mutex_unlock(&clientListMutex);	
	
	map_free(priv->map);
	free(data);
	pthread_exit(NULL);
	// This won't be called in reality but let the compiler think it will be
	return 0;
}


int client_getNextEvent(char** currentEvent, client_publicData* data) {
	int currentPosition = *currentEvent - data->buf;
	int lastPosition    = data->bufLen - ((*currentEvent)[1] + 1 + 1);
	if (currentPosition < lastPosition) {
		*currentEvent += (*currentEvent)[1] + 2;
		return 1;
	}
	
	return 0;
}


void* client_process(void* dataPointer) {
	client_publicData* data = dataPointer;
	
	//pthread_mutex_lock(&clientsMutex);
	//currentClients++;
	//pthread_mutex_unlock(&clientsMutex);
	
	client_privateData private;
	private.previousTick = 0;
	gettimeofday(&private.lastPacketTime, NULL);
	// Spawn player at the start (right after he joins)
	private.spawningPlayer = 1;
	private.spawnTimerStart = private.lastPacketTime;
	private.map = map_clone();
	
	for (;;) {
		pthread_mutex_lock(&data->mutex);
		if (data->bufHasNewData) {
			private.currentTick = data->buf[0];

			if (isPacketNewer(private.currentTick, &private.previousTick)) {
				char* currentEvent = data->buf + 1;
				
				do {
					initBinaryReader(currentEvent + 2);

					switch(*currentEvent) {
						case NET_EVENT_CLIENT_EXIT:
							printf("EVENT_CLIENT_EXIT %hhu\n", data->id);
							return client_stop(data, &private);
						case NET_EVENT_PLAYER_DIED: {
							player_kill(data->id);
							break;
						}
						case NET_EVENT_PLAYER_MOVED: {
							float  x = binaryReadFloat();
							float  y = binaryReadFloat();
							float vx = binaryReadFloat();
							float vy = binaryReadFloat();
							if (!map_collides(private.map, x, y, PLAYER_WIDTH, PLAYER_HEIGHT)
							 && player_moved(data->id, x, y, vx, vy))
								srv_addNewEvent(NET_EVENT_PLAYER_MOVED, "1ffff", data->id, x, y, vx, vy);
							else {
								float x;
								float y;
								player_getPos(data->id, &x, &y);
								srv_addNewEvent(NET_EVENT_PLAYER_MOVE_DENIED, "1ff", data->id, x, y);
							}
							break;
						}
						case NET_EVENT_PLAYER_JUMP: {
							srv_addNewEvent(NET_EVENT_PLAYER_JUMP, "1", data->id);
							break;
						}
						case NET_EVENT_PLAYER_ATTACK: {
							srv_addNewEvent(NET_EVENT_PLAYER_ATTACK, "1", data->id);
							break;
						}
					}
				} while (client_getNextEvent(&currentEvent, data));
				
				gettimeofday(&private.lastPacketTime, NULL);
			}
			
			data->bufHasNewData = 0;
		}
		pthread_mutex_unlock(&data->mutex);
		
		double timerDiff = getMsDifference(private.lastPacketTime);
		if (timerDiff > MS_TO_TIMEOUT) {
			printf("TIMEOUTED %hhu\n", data->id);
			return client_stop(data, &private);
		}
		
		if (private.spawningPlayer) {
			timerDiff = getMsDifference(private.spawnTimerStart);
			if (timerDiff > MS_TO_SPAWN) {
				private.spawningPlayer = 0;
				float x, y;
				map_getSpawnPosition(private.map, &x, &y, PLAYER_WIDTH, PLAYER_HEIGHT);
				player_spawn(data->id, x, y);
				srv_addNewEvent(NET_EVENT_PLAYER_SPAWN, "1ff", data->id, x, y);
			}
		}
		
		usleep(1);
	}
	
	return client_stop(data, &private);
}


//////////////////////
// PUBLIC FUNCTIONS //
//////////////////////

uint32_t client_create(struct sockaddr_in client_address) {
	unsigned int id = MAX_CLIENTS;
	
	pthread_mutex_lock(&clientListMutex);
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (clients[i].cd == NULL) {
			id = i;
			break;
		}
	}
	pthread_mutex_unlock(&clientListMutex);
	
	// No more slots
	if (id == MAX_CLIENTS)
		return id;
	
	client_publicData* data = (client_publicData*)malloc(sizeof(client_publicData));
	*(unsigned int*)&data->id = id;
	pthread_mutex_init(&(data->mutex), NULL);
	data->bufHasNewData = 0;
	
	int rc = pthread_create((pthread_t*)&(data->thread), NULL, client_process, (void*)data);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		return MAX_CLIENTS;
	}
	
	pthread_mutex_lock(&clientListMutex);
	clients[id].cd = data;
	clients[id].address = client_address;
	pthread_mutex_unlock(&clientListMutex);
	
	return id;
}


void client_transferPacket(struct sockaddr_in client_address, char* data, int dataLen) {
	pthread_mutex_lock(&clientListMutex);
	for (int i = 0;i < MAX_CLIENTS; ++i) {
		if (clients[i].cd == NULL)
			continue;
		
		if (clients[i].address.sin_addr.s_addr == client_address.sin_addr.s_addr
		 && clients[i].address.sin_port        == client_address.sin_port) {
			pthread_mutex_lock(&(clients[i].cd->mutex));
			memcpy(clients[i].cd->buf, data, dataLen);
			clients[i].cd->bufLen = dataLen;
			clients[i].cd->bufHasNewData = 1;
			pthread_mutex_unlock(&(clients[i].cd->mutex));
		}
	}
	pthread_mutex_unlock(&clientListMutex);
}