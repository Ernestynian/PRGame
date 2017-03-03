#ifndef CLIENT_H
#define CLIENT_H

#define CLIENT_NOT_CREATED MAX_CLIENTS

uint32_t client_create(struct sockaddr_in client_address);
void client_transferPacket(struct sockaddr_in client_address, char* data);

extern unsigned int currentClients;
extern pthread_mutex_t clientsMutex;

#endif /* CLIENT_H */

