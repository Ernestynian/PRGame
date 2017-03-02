#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "../../Common/networkInterface.h"
#include "../../Common/byteConverter.h"


typedef struct {
	const pthread_t thread;	
	const unsigned int id;
	
	char buf[MAX_PACKET_SIZE];
	char bufHasNewData;
	pthread_mutex_t bufMutex;
	
	// PRIVATE RESOURCES
	unsigned char previousTick;
} client_data;


struct {
	client_data* cd;
	struct sockaddr_in address;
} clients[MAX_CLIENTS];
pthread_mutex_t clientListMutex = PTHREAD_MUTEX_INITIALIZER;

unsigned int currentClients = 0;
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;


int isPacketNewer(client_data* data) {
	unsigned char tick = data->buf[0];
	unsigned char diff = tick - data->previousTick;
	data->previousTick = tick;
	
	if (diff >= 1 && diff <= 100)
		return 1;
	
	diff += 255;
	if (diff >= 1 && diff <= 100)
		return 1;
	
	return 0;
}


void* client_stop(client_data* data) {
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
	client_data* data = (client_data*)dataPointer;
	//pthread_mutex_lock(&data->mutex);
	data->previousTick = 0;
	
	printf("Hello World! It's me, thread #%ld!\n", data->thread);
	//pthread_mutex_unlock(&data->mutex);
	
	pthread_mutex_lock(&clientsMutex);
	currentClients++;
	pthread_mutex_unlock(&clientsMutex);
	
	clock_t lastPacketTime = clock(), diff;
	for (;;) {
		pthread_mutex_lock(&data->bufMutex);
		if (data->bufHasNewData) {
			if (data->buf[1] == EVENT_CLIENT_EXIT) {
				printf("EVENT_CLIENT_EXIT #%ld\n", data->thread);
				return client_stop(data);
			}
			
			printf("[%3hhu] ", data->buf[0]);
			if (isPacketNewer(data)) {
				if (data->buf[1] > 0) {
					printf("received message: ");
					if (data->buf[1] == EVENT_PLAYER_MOVED) {
						initBinaryReader(data->buf + 3);
						int a = binaryRead4B();
						int b = binaryRead4B();
						printf(" %d %d", a, b);
					} else 
						printf("\"%s\"", data->buf + 3);

					printf(" [%d %d]\n", data->buf[1], data->buf[2]);
				} else
					printf("received ping message: %d\n", data->buf[1]);
				
				lastPacketTime = clock();
			}

			data->bufHasNewData = 0;
		}
		pthread_mutex_unlock(&data->bufMutex);
		
		diff = (clock() - lastPacketTime) * 1000 / CLOCKS_PER_SEC;
		if (diff > MS_TO_TIMEOUT) {
			printf("TIMEOUTED #%ld\n", data->thread);
				return client_stop(data);
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
	
	client_data* data = (client_data*)malloc(sizeof(client_data));
	*(unsigned int*)&data->id = id;
	pthread_mutex_init(&(data->bufMutex), NULL);
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
			pthread_mutex_lock(&(clients[i].cd->bufMutex));
			// TODO: change const length into dynamic
			memcpy(clients[i].cd->buf, data, MAX_PACKET_SIZE);
			clients[i].cd->bufHasNewData = 1;
			pthread_mutex_unlock(&(clients[i].cd->bufMutex));
		}
	}
	pthread_mutex_unlock(&clientListMutex);
}