#include <stdlib.h>
#include <string.h>
#include "networkInterface.h"


struct NetworkEvent* createEvent(char type, const char* data, int dataLength) {
	struct NetworkEvent* event = (struct NetworkEvent*)malloc(sizeof(struct NetworkEvent));
	
	event->length = 2 + dataLength;
	
	event->data = malloc(event->length);
	
	event->data[0] = type;
	event->data[1] = dataLength;
	memcpy(event->data + 2, data, dataLength);
	
	return event;
};


void releaseEvent(struct NetworkEvent* event) {
	free(event->data);
	free(event);
}