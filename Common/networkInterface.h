/* Network Interface Specification
 * 
 * Packet consists of tick ID (1 ~ 255) of the current second
 *  and list of events
 * Tick of ID 0 is reserved for special packets
 * 
 * Event is built as Byte(tick) Byte(Type) Byte(data length) Bytes(data)
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
	NET_EVENT_PING = 42,
	
	NET_EVENT_CLIENT_JOIN,
	NET_EVENT_CLIENT_ACCEPTED,
	NET_EVENT_CLIENT_EXIT,
	
	NET_EVENT_PLAYER_SPAWN,
	NET_EVENT_PLAYER_LEFT,
	NET_EVENT_PLAYER_DIED,
	NET_EVENT_PLAYER_MOVED
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