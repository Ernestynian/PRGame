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


Client clients[MAX_CLIENTS];
pthread_mutex_t clientListMutex = PTHREAD_MUTEX_INITIALIZER;


void* client_stop(client_PublicData* data, client_privateData* priv) {
	srv_addNewEvent(NET_EVENT_CLIENT_EXIT, "1", data->id);
	
	pthread_mutex_lock(&data->packets_mutex);
	Packet* p = data->packets;
	do {
		free((char*)p->data);
		free(p);
		p = p->next;
	} while (p != NULL);
	pthread_mutex_unlock(&data->packets_mutex);	
	
	player_kill(data->id);
	
	pthread_mutex_lock(&clientListMutex);
	clients[data->id].pd = NULL;
	pthread_mutex_unlock(&clientListMutex);	
	
	map_free(priv->map);
	free(data);
	pthread_exit(NULL);
	// This won't be called in reality but let the compiler think it will be
	return 0;
}


int client_getNextEvent(const char** currentEvent, Packet* packet) {
	if (packet->length < 3)
		return 0;
	
	int currentPosition = *currentEvent - packet->data;
	int lastPosition    = packet->length - ((*currentEvent)[1] + 1 + 1);
	if (currentPosition < lastPosition) {
		*currentEvent += (*currentEvent)[1] + 2;
		return 1;
	}
	
	return 0;
}


void* client_process(void* threadData) {
	void** dataArray = threadData;
	client_PublicData* public = (client_PublicData*)(dataArray[0]);
	
	player_reset(public->id);
	
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
		pthread_mutex_lock(&public->packets_mutex);
		Packet* packet = public->packets;
		pthread_mutex_unlock(&public->packets_mutex);
		
		if (packet != NULL) {
			do {
				private.currentTick = packet->data[0];
				
				if (isPacketNewer(private.currentTick, &private.previousTick)) {
					const char* currentEvent = packet->data + 1;

					do {
						initBinaryReader(currentEvent + 2);
						
						switch(currentEvent[0]) {
							case NET_EVENT_CLIENT_EXIT:
								printf("EVENT_CLIENT_EXIT %hhu\n", public->id);
								return client_stop(public, &private);
								
							case NET_EVENT_PLAYER_DIED:
								player_kill(public->id);
								break;
								
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
							
							case NET_EVENT_PLAYER_JUMP:
								srv_addNewEvent(NET_EVENT_PLAYER_JUMP, "1", public->id);
								break;
								
							case NET_EVENT_PLAYER_ATTACK:
								if (!private.attacking) {
									private.attacking = 1;
									gettimeofday(&private.attackTimerStart, NULL);
									srv_addNewEvent(NET_EVENT_PLAYER_ATTACK, "1", public->id);
								}
								break;
						}
					} while (client_getNextEvent(&currentEvent, packet));

					gettimeofday(&private.lastPacketTime, NULL);
				}
				
				free((char*)packet->data);

				pthread_mutex_lock(&public->packets_mutex);
				public->packets = packet->next;
				
				free(packet);
				
				packet = public->packets;
				pthread_mutex_unlock(&public->packets_mutex);
			} while (packet != NULL);
		}
		
		double timerDiff = getMsDifference(private.lastPacketTime);
		if (timerDiff > MS_TO_TIMEOUT) {
			printf("TIMEOUTED %hhu\n", public->id);
			return client_stop(public, &private);
		}
		
		if (private.attacking) {
			timerDiff = getMsDifference(private.attackTimerStart);
			if (timerDiff > MS_TO_ATTACK) {
				private.attacking = 0;
				
				// Check if killed someone
				float x, y;
				int w = 10, h = 5;
				player_getPos(public->id, &x, &y);
				
				if (player_isLookingRight(public->id))
					x += PLAYER_WIDTH;
				else
					x -= w;
				
				y += 10;
				
				for (int i = 0; i < MAX_CLIENTS; ++i) {
					if (i == public->id)
						continue;
					
					if (player_isAlive(i)) {
						if (player_collides(i, x, y, w, h)) {
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
		if (clients[i].pd == NULL) {
			id = i;
			break;
		}
	}
	pthread_mutex_unlock(&clientListMutex);
	
	// No more slots
	if (id == MAX_CLIENTS)
		return MAX_CLIENTS;
	
	
	client_PublicData* data = malloc(sizeof(client_PublicData));
	*(unsigned int*)&data->id = id;
	pthread_mutex_init(&(data->packets_mutex), NULL);
	data->packets = NULL;
	
	void** threadData = malloc(sizeof(void*) * 2);
	threadData[0] = (void*)data;
	threadData[1] = (void*)mapClone;
	
	pthread_mutex_lock(&clientListMutex);
	clients[id].pd = data;
	clients[id].address = client_address;
	pthread_mutex_unlock(&clientListMutex);
	
	int rc = pthread_create((pthread_t*)&(data->thread), NULL, client_process, (void*)threadData);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		return MAX_CLIENTS;
	}
	
	return id;
}


void client_transferPacket(struct sockaddr_in client_address, char* data, int dataLen) {
	Packet* packet = malloc(sizeof(Packet));
	*(char**)&packet->data = malloc(dataLen);
	memcpy((char*)packet->data, data, dataLen);
	*(int*)&packet->length = dataLen;
	packet->next = NULL;
	
	pthread_mutex_lock(&clientListMutex);
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (clients[i].pd == NULL)
			continue;
		
		if (clients[i].address.sin_addr.s_addr == client_address.sin_addr.s_addr
		 && clients[i].address.sin_port        == client_address.sin_port) {
			pthread_mutex_lock(&(clients[i].pd->packets_mutex));
			
			Packet* curPacket = clients[i].pd->packets;
			if (curPacket == NULL)
				clients[i].pd->packets = packet;
			else {
				while (curPacket->next != NULL)
					curPacket = curPacket->next;
				
				curPacket->next = packet;
			}
			
			pthread_mutex_unlock(&(clients[i].pd->packets_mutex));
			break;
		}
	}
	pthread_mutex_unlock(&clientListMutex);
}