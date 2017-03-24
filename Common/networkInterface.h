/* Network Interface Specification
 * 
 * Packet consists of tick ID (1 ~ 255) of the current second
 *  and list of events
 * Tick of ID 0 is reserved for special packets - always accept
 * 
 * Event is built as Byte(tick) Byte(Type) Byte(data length) Bytes(data)
 * 
 */

#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#define PORT 1337
#define MAX_CLIENT_PACKET_SIZE 256
#define MAX_SERVER_PACKET_SIZE 2048
#define MAX_CLIENTS 32

#define MS_TO_TIMEOUT 1000
#define MS_TO_SPAWN   2000
#define MS_TO_ATTACK  50

#define PLAYER_MAX_MOVEMENT_SPEED 0.15
#define PLAYER_ACCELERATION 0.008
#define GRAVITY_CONSTANT 0.0005
#define JUMP_ACCELERATION -0.35

#define FRAMERATE 60

#ifdef __cplusplus
extern "C" {
#endif
	
// Comments include information about who can send - [Server / Client], 
//  and event parameters - <size:name>
enum EventTypes {	
	// [C] none
	NET_EVENT_PING = 0,
	
	// [S] <1:new client id>
	// [C] none
	NET_EVENT_CLIENT_JOIN,
	// [S] <1:new client id>
	NET_EVENT_CLIENT_ACCEPTED,
	// [S] <1:client id> <1:icons amount> <2:icon x> <2:icon y> <1:icon tex id> (...)
	// [C] none
	NET_EVENT_CLIENT_EXIT,
	
	// [S] <1:client id> <4:pos_x> <4:pos_y>
	NET_EVENT_PLAYER_SPAWN,
	// [S] <1:client id>
	NET_EVENT_PLAYER_DIED,
	// [S] <1:client id> <4:pos_x> <4:pos_y>
	// [C] <4:pos_x> <4:pos_y>
	NET_EVENT_PLAYER_MOVED,
	// [S] <1:client id> <4:pos_x> <4:pos_y>
	NET_EVENT_PLAYER_MOVE_DENIED,
	// [S] <1:client id>
	// [C] none
	NET_EVENT_PLAYER_JUMP,
    
	NET_EVENT_PLAYER_ATTACK
};


typedef struct {
	char  length;
	// up to 255 bytes
	char* data;	
} NetworkEvent;


NetworkEvent* createEvent(char type, const char* data, int dataLength);
void releaseEvent(NetworkEvent* event);

int isPacketNewer(unsigned char currentTick, unsigned char* previousTick);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_INTERFACE_H */