#include <stdlib.h>
#include <string.h>
#include "networkInterface.h"


NetworkEvent* createEvent(char type, const char* data, int dataLength) {
	NetworkEvent* event = (NetworkEvent*)malloc(sizeof(NetworkEvent));
	
	event->length = 2 + dataLength;
	
	event->data = malloc(event->length);
	
	event->data[0] = type;
	event->data[1] = dataLength;
	memcpy(event->data + 2, data, dataLength);
	
	return event;
};


void releaseEvent(NetworkEvent* event) {
	free(event->data);
	free(event);
}