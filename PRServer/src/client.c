#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "../../Common/networkInterface.h"
#include "../../Common/byteConverter.h"

#include "server.h"
#include "client.h"
#include "players.h"


typedef struct {
	unsigned char currentTick;
	unsigned char previousTick;
	
	clock_t lastPacketTime;
	int spawningPlayer;
	clock_t spawnTimerStart;
} client_privateData;


client clients[MAX_CLIENTS];
pthread_mutex_t clientListMutex = PTHREAD_MUTEX_INITIALIZER;

unsigned int currentClients = 0;
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;


void* client_stop(client_publicData* data) {
	srv_addNewEvent(NET_EVENT_CLIENT_EXIT, "1", data->id);
	
	pthread_mutex_lock(&clientsMutex);
	currentClients--;
	pthread_mutex_unlock(&clientsMutex);
	
	pthread_mutex_lock(&clientListMutex);
	clients[data->id].cd = NULL;
	pthread_mutex_unlock(&clientListMutex);	
	
	free(data);
	pthread_exit(NULL);
	// This won't be called in reality but let the compiler think it will be
	return 0;
}




void* client_process(void* dataPointer) {
	client_publicData* data = dataPointer;
	
	pthread_mutex_lock(&clientsMutex);
	currentClients++;
	pthread_mutex_unlock(&clientsMutex);
	
	client_privateData private;
	private.previousTick = 0;
	private.lastPacketTime = clock();
	// Spawn player at the start (right after he joins)
	private.spawningPlayer = 1;
	private.spawnTimerStart = private.lastPacketTime;
	
	for (;;) {
		pthread_mutex_lock(&data->mutex);
		if (data->bufHasNewData) {
			if (data->buf[1] == NET_EVENT_CLIENT_EXIT) {
				printf("EVENT_CLIENT_EXIT %d\n", data->id);
				return client_stop(data);
			}
			
			private.currentTick = data->buf[0];
			
			if (isPacketNewer(private.currentTick, &private.previousTick)) {
				//printf("[%3hhu] ", data->buf[0]);
				if (data->buf[1] > 0) {
					printf("received message: ");
					
					initBinaryReader(data->buf + 3);
					
					switch(data->buf[1]) {
						case NET_EVENT_PLAYER_MOVED:
							player_moved(data->id, data->buf + 3);
							break;
						default:
							printf("\"%s\"", data->buf + 3);
							break;
					}						

					printf(" [%d %d]\n", data->buf[1], data->buf[2]);
				} //else printf("received ping message: %d\n", data->buf[1]);
				
				private.currentTick = data->buf[0];
			}
			
			private.lastPacketTime = clock();
			data->bufHasNewData = 0;
		}
		pthread_mutex_unlock(&data->mutex);
		
		clock_t timerDiff = (clock() - private.lastPacketTime) * 1000 / CLOCKS_PER_SEC;
		if (timerDiff > MS_TO_TIMEOUT) {
			printf("TIMEOUTED %d\n", data->id);
			return client_stop(data);
		}
		
		if (private.spawningPlayer) {
			timerDiff = (clock() - private.spawnTimerStart) * 1000 / CLOCKS_PER_SEC;
			if (timerDiff > MS_TO_SPAWN) {
				private.spawningPlayer = 0;
				printf("SPAWN REQUESTED\n");
				// TODO: randomize and not collide with map
				int x = 20 + 30 * data->id;
				int y = 40;
				player_spawn(data->id, x, y);
				srv_addNewEvent(NET_EVENT_PLAYER_SPAWN, "144", data->id, x, y);
			}
		}
	}
	
	return client_stop(data);
}


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


void client_transferPacket(struct sockaddr_in client_address, char* data) {
	pthread_mutex_lock(&clientListMutex);
	for (int i = 0;i < MAX_CLIENTS; ++i) {
		if (clients[i].cd == NULL)
			continue;
		
		if (clients[i].address.sin_addr.s_addr == client_address.sin_addr.s_addr
		 && clients[i].address.sin_port        == client_address.sin_port) {
			pthread_mutex_lock(&(clients[i].cd->mutex));
			// TODO: change const length into dynamic
			memcpy(clients[i].cd->buf, data, MAX_CLIENT_PACKET_SIZE);
			clients[i].cd->bufHasNewData = 1;
			pthread_mutex_unlock(&(clients[i].cd->mutex));
		}
	}
	pthread_mutex_unlock(&clientListMutex);
}