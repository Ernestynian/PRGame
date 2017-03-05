#ifndef NETWORK_H
#define NETWORK_H

#include <string>

#include <SDL2/SDL_net.h>

#include "../../Common/networkInterface.h"


class Network {
public:
	Network();
	~Network();
	
	void checkForOneNewPacket();
	EventTypes getNextEvent();
	uint8_t* getCurrentEventData();
	
	bool recieviedAcceptMessage();
	
	void addNewEvent(EventTypes eventType, const char* format, ...);
	bool sendPacket(unsigned char frameTime);
private:
	bool init();
	bool setServer(const char* host, uint16_t port);
	bool createPackets();
	
	void sendEvent(EventTypes eventType, const char* data, int length);
	
	bool initialized;
	
	UDPsocket  UDPSocket;
	IPaddress  serverAddress;
	UDPpacket* packetOut;
	UDPpacket* packetIn;
	UDPpacket* packetDirect;

	uint8_t*   currentEvent;
	
	const char* LOCALHOST = "127.0.0.1";
};

#endif /* NETWORK_H */

