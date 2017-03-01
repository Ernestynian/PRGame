#ifndef NETWORK_H
#define NETWORK_H

#include <string>

#include <SDL2/SDL_net.h>

#include "../../Common/networkInterface.h"

class Network {
public:
	Network();
	~Network();
	
	void checkForData();
	
	void addEvent(EventTypes eventType, const char* data, int length);
	bool sendPacket();
private:
	bool init();
	bool setServer(const char* host, uint16_t port);
	bool createPackets(int32_t packetSize);
	
	bool sendEvent(EventTypes eventType, const std::string& str);
	
	bool initialized;
	
	UDPsocket UDPSocket;
	IPaddress serverAddress;
	UDPpacket* packetOut;
	UDPpacket* packetIn;
	UDPpacket* packetIndep;
	
	const char* LOCALHOST = "127.0.0.1";
};

#endif /* NETWORK_H */

