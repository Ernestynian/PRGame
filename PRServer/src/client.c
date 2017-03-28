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
#include "map.h"
#include "client.h"
#include "players.h"


typedef struct {
	unsigned char currentTick;
	unsigned char previousTick;
	
	MapData* map;
	
	struct timeval lastPacketTime;
	int spawningPlayer;
	struct timeval spawnTimerStart;
	
	struct timeval attackTimerStart;
	int attacking;
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
	int currentPosition = *currentEvent - data->packetData;
	int lastPosition    = data->packetLength - ((*currentEvent)[1] + 1 + 1);
	if (currentPosition < lastPosition) {
		*currentEvent += (*currentEvent)[1] + 2;
		return 1;
	}
	
	return 0;
}


void* client_process(void* threadData) {
	void** dataArray = threadData;
	client_publicData* public = (client_publicData*)(dataArray[0]);
	
	//pthread_mutex_lock(&clientsMutex);
	//currentClients++;
	//pthread_mutex_unlock(&clientsMutex);
	
	client_privateData private;
	private.previousTick = 0;
	gettimeofday(&private.lastPacketTime, NULL);
	// Spawn player at the start (right after he joins)
	private.spawningPlayer = 1;
	private.attacking = 0;
	private.spawnTimerStart = private.lastPacketTime;
	private.map = (MapData*)(dataArray[1]);
	
	free(threadData);
	
	for (;;) {
		pthread_mutex_lock(&public->mutex);
		if (public->packetStatus) {
			private.currentTick = public->packetData[0];

			if (isPacketNewer(private.currentTick, &private.previousTick)) {
				char* currentEvent = public->packetData + 1;
				
				do {
					initBinaryReader(currentEvent + 2);

					switch(*currentEvent) {
						case NET_EVENT_CLIENT_EXIT:
							printf("EVENT_CLIENT_EXIT %hhu\n", public->id);
							return client_stop(public, &private);
						case NET_EVENT_PLAYER_DIED: {
							player_kill(public->id);
							break;
						}
						case NET_EVENT_PLAYER_MOVED: {
							float  x = binaryReadFloat();
							float  y = binaryReadFloat();
							float vx = binaryReadFloat();
							float vy = binaryReadFloat();
							
							int colX = x + PLAYER_X_OFFSET;
							int colY = y + PLAYER_Y_OFFSET;
							int canMove = !map_collides(private.map, colX, colY, PLAYER_WIDTH, PLAYER_HEIGHT);
							if (canMove && player_moved(public->id, x, y, vx, vy))
								srv_addNewEvent(NET_EVENT_PLAYER_MOVED, "1ffff", public->id, x, y, vx, vy);
							else {
								float x, y;
								player_getPos(public->id, &x, &y);
								player_stopMovement(public->id);
								srv_addNewEvent(NET_EVENT_PLAYER_MOVE_DENIED, "1ff", public->id, x, y);
							}
							break;
						}
						case NET_EVENT_PLAYER_JUMP: {
							srv_addNewEvent(NET_EVENT_PLAYER_JUMP, "1", public->id);
							break;
						}
						case NET_EVENT_PLAYER_ATTACK: {
							if (!private.attacking) {
								private.attacking = 1;
								gettimeofday(&private.attackTimerStart, NULL);
								srv_addNewEvent(NET_EVENT_PLAYER_ATTACK, "1", public->id);
							}
							break;
						}
					}
				} while (client_getNextEvent(&currentEvent, public));
				
				gettimeofday(&private.lastPacketTime, NULL);
			}
			
			public->packetStatus = 0;
		}
		pthread_mutex_unlock(&public->mutex);
		
		double timerDiff = getMsDifference(private.lastPacketTime);
		if (timerDiff > MS_TO_TIMEOUT) {
			printf("TIMEOUTED %hhu\n", public->id);
			return client_stop(public, &private);
		}
		
		if (private.attacking) {
			timerDiff = getMsDifference(private.attackTimerStart);
			if (timerDiff > MS_TO_ATTACK) {
				private.attacking = 0;
				// check if killed someone
				float x, y;
				player_getPos(public->id, &x, &y);
				// TODO: more precise values of the hand
				x += PLAYER_WIDTH;
				y += 10;
				for (int i = 0; i < MAX_CLIENTS; ++i) {
					if (i == public->id)
						continue;
					
					if (player_isAlive(i)) {
						if (player_collides(i, x, y, 10, 5)) {
							srv_addNewEvent(NET_EVENT_PLAYER_DIED, "1", i);
							player_kill(i);
						}
					}
				}
			}
		}
		
		if (private.spawningPlayer) {
			timerDiff = getMsDifference(private.spawnTimerStart);
			if (timerDiff > MS_TO_SPAWN) {
				private.spawningPlayer = 0;
				float x, y;
				map_getSpawnPosition(private.map, &x, &y, PLAYER_WIDTH, PLAYER_HEIGHT);
				x -= PLAYER_X_OFFSET;
				y -= PLAYER_Y_OFFSET;
				player_spawn(public->id, x, y);
				srv_addNewEvent(NET_EVENT_PLAYER_SPAWN, "1ff", public->id, x, y);
			}
		} else if (!player_isAlive(public->id)) {
			private.spawningPlayer = 1;
			gettimeofday(&private.spawnTimerStart, NULL);
		}
		
		usleep(1);
	}
	
	return client_stop(public, &private);
}


//////////////////////
// PUBLIC FUNCTIONS //
//////////////////////

uint32_t client_create(struct sockaddr_in client_address, MapData* mapClone) {
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
		return MAX_CLIENTS;
	
	
	client_publicData* data = malloc(sizeof(client_publicData));
	*(unsigned int*)&data->id = id;
	pthread_mutex_init(&(data->mutex), NULL);
	data->packetStatus = 0;
	
	void** threadData = malloc(sizeof(void*) * 2);
	threadData[0] = (void*)data;
	threadData[1] = (void*)mapClone;
	
	int rc = pthread_create((pthread_t*)&(data->thread), NULL, client_process, (void*)threadData);
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


// FIXME: what if packet is not parsed and another one comes?
void client_transferPacket(struct sockaddr_in client_address, char* data, int dataLen) {
	pthread_mutex_lock(&clientListMutex);
	for (int i = 0;i < MAX_CLIENTS; ++i) {
		if (clients[i].cd == NULL)
			continue;
		
		if (clients[i].address.sin_addr.s_addr == client_address.sin_addr.s_addr
		 && clients[i].address.sin_port        == client_address.sin_port) {
			pthread_mutex_lock(&(clients[i].cd->mutex));
			memcpy(clients[i].cd->packetData, data, dataLen);
			clients[i].cd->packetLength = dataLen;
			clients[i].cd->packetStatus = 1;
			pthread_mutex_unlock(&(clients[i].cd->mutex));
		}
	}
	pthread_mutex_unlock(&clientListMutex);
}