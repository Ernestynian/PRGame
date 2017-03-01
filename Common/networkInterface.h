#define PORT 1337
#define PACKET_SIZE 512

#ifdef __cplusplus
extern "C" {
#endif

enum EventTypes {
	EVENT_PING = 0,
	
	EVENT_CLIENT_JOIN,
	EVENT_CLIENT_EXIT,
	
	EVENT_PLAYER_SPAWN,
	EVENT_PLAYER_LEFT,
	EVENT_PLAYER_DIED,
	EVENT_PLAYER_MOVED
};


struct NetworkEvent {
	// up to 255 bytes
	char* data;
	int   length;
};


struct NetworkEvent* createEvent(char type, const char* data, int dataLength);
void releaseEvent(struct NetworkEvent* event);


#ifdef __cplusplus
}
#endif