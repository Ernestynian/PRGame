#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <thread>
#include <queue>
#include <mutex>

#include <SDL2/SDL_net.h>

#include "../../Common/networkInterface.h"


class Network {
public:
	Network();
	~Network();
	
	bool openNextPacket();
    bool receivePacket();
    void discardOpenedPacket();
	bool isThereMoreEvents();
	EventTypes getNextEvent();
	EventTypes getCurrentEventDataType();
	char getCurrentEventDataLength();
	uint8_t* getCurrentEventData();
	
	bool receivedAcceptMessage();
	
	void addNewEvent(EventTypes eventType, const char* format, ...);
	void addNewEvent(EventTypes eventType);
	bool sendPacket(unsigned char frameTime);
    void start();
    int getPacketCount();
private:
	bool init();
	bool setServer(const char* host, uint16_t port);
	bool createPackets();
        
        void networkThread(unsigned int tickrate);
	
	void sendEvent(EventTypes eventType, const char* data, int length);
	
	bool initialized;
	
	UDPsocket  UDPSocket;
	IPaddress  serverAddress;
	UDPpacket* packetOut;
	UDPpacket* packetIn;
	UDPpacket* packetDirect;
	unsigned char previousServerTick;
	
	uint8_t*   currentEvent;
	
	const char* LOCALHOST = "127.0.0.1";
        
	unsigned char frame;
    std::thread netThread;
    
    std::queue<UDPpacket*> packetQueue;
    std::mutex packetQueueMutex;
};

#endif /* NETWORK_H */

