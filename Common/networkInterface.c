#include <stdlib.h>
#include <string.h>
#include "networkInterface.h"


NetworkEvent* createEvent(char type, const char* data, int dataLength) {	
	NetworkEvent* event = (NetworkEvent*)malloc(sizeof(NetworkEvent));
	
	event->length = 2 + dataLength;
	
	event->data = malloc(event->length);
	
	event->data[0] = type;
	event->data[1] = dataLength;
	if (data != NULL && dataLength > 0)
		memcpy(event->data + 2, data, dataLength);
	
	return event;
};


void releaseEvent(NetworkEvent* event) {
	free(event->data);
	free(event);
}

int isPacketNewer(unsigned char currentTick, unsigned char* previousTick) {
	// Always accept packets with tick 0
	if (currentTick == 0)
		return 1;
	
	// Always accept when network is not "initialized"
	if (*previousTick == 0) {
		*previousTick = currentTick;
		return 1;
	}
	
	unsigned char diff = currentTick - *previousTick;
	*previousTick = currentTick;
	
	if (diff >= 1 && diff <= 100)
		return 1;
	
	diff += 255;
	if (diff >= 1 && diff <= 100)
		return 1;
	
	return 0;
}