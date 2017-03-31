#ifndef SERVER_H
#define SERVER_H

#include "../../Common/networkInterface.h"

typedef struct sockaddr Address;
typedef struct sockaddr_in Address_in;


int  srv_start();
void srv_stop();
int  srv_transferPackets();
void srv_addNewEvent(char eventType, const char* format, ...);

#endif /* SERVER_H */