/* Network Interface Specification
 * 
 * Packet consists of tick ID (1 ~ 255) of current second
 *  and list of events
 * 
 * Event is built as Byte(Type) Byte(data length) Bytes(data)
 * 
 */

#define PORT 1337
#define MAX_PACKET_SIZE 512
#define MAX_CLIENTS 32

#define MS_TO_TIMEOUT 1000

#ifdef __cplusplus
extern "C" {
#endif

enum EventTypes {
	EVENT_PING = 0,
	
	EVENT_CLIENT_JOIN,
	EVENT_CLIENT_ACCEPTED,
	EVENT_CLIENT_EXIT,
	
	EVENT_PLAYER_SPAWN,
	EVENT_PLAYER_LEFT,
	EVENT_PLAYER_DIED,
	EVENT_PLAYER_MOVED
};


typedef struct {
	char  length;
	// up to 255 bytes
	char* data;	
} NetworkEvent;


NetworkEvent* createEvent(char type, const char* data, int dataLength);
void releaseEvent(NetworkEvent* event);


#ifdef __cplusplus
}
#endif