#ifndef SERVER_H
#define SERVER_H

#include "../../Common/networkInterface.h"

int  srv_start();
void srv_stop();
int  srv_transferPackets();
void srv_addNewEvent(char eventType, const char* format, ...);

#endif /* SERVER_H */