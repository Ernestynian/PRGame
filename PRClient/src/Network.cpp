#include "Network.h"


Network::Network() {
	if (!init()
	 || !setServer(LOCALHOST, PORT)
	 || !createPackets(MAX_PACKET_SIZE))
		initialized = false;
	
	initialized = true;
	
	sendEvent(EVENT_CLIENT_JOIN, 0, 0);
}


Network::~Network() {
	if(initialized) {
		// Notify Server
		packetOut->len = 0;
		sendEvent(EVENT_CLIENT_EXIT, 0, 0);
		
		// Clean up
		SDLNet_FreePacket(packetIn);
		SDLNet_Quit();
	}
}


bool Network::init() {
	if(SDLNet_Init() == -1) {
		printf("SDLNet_Init failed: %s", SDLNet_GetError());
		return false;
	}

	UDPSocket = SDLNet_UDP_Open(0);
	if(UDPSocket == nullptr) {
		printf("SDLNet_UDP_Open failed: %s", SDLNet_GetError());
		return false;
	}
	
	return true;
}


/**
 * Set destination IP and port number with correct endianess
 * 
 * @return true on success
 */
bool Network::setServer(const char* host, uint16_t port) {
	if(SDLNet_ResolveHost(&serverAddress, host, port) == -1) {
		printf("SDLNet_ResolveHost failed: %s", SDLNet_GetError());
		return false;
	}
	
	return true;
}


bool Network::createPackets(int32_t packetSize) {
	packetIn = SDLNet_AllocPacket(packetSize);

	if(packetIn == nullptr) {
		printf("SDLNet_AllocPacket failed: %s", SDLNet_GetError());
		return false;
	}
	
	
	packetOut = SDLNet_AllocPacket(packetSize);

	if(packetOut == nullptr) {
		printf("SDLNet_AllocPacket failed: %s", SDLNet_GetError());
		return false;
	}

	packetOut->address.host = serverAddress.host;
	packetOut->address.port = serverAddress.port;
	packetOut->len          = 1;
	
	
	packetIndep = SDLNet_AllocPacket(packetSize);

	if(packetIndep == nullptr) {
		printf("SDLNet_AllocPacket failed: %s", SDLNet_GetError());
		return false;
	}
	
	packetIndep->address.host = serverAddress.host;
	packetIndep->address.port = serverAddress.port;
	
	return true;
}


/**
 * Append a new event to the packet
 */
void Network::addEvent(EventTypes eventType, const char* data, int length) {
	NetworkEvent* event = createEvent(eventType, data, length);
	
	if (packetOut->len + event->length < packetOut->maxlen) {
		memcpy(packetOut->data + packetOut->len, event->data, event->length);
		packetOut->len += event->length;
	}
	
	releaseEvent(event);
}


void Network::sendEvent(EventTypes eventType, const char* data, int length) {
	NetworkEvent* event = createEvent(eventType, data, length);
	
	memcpy(packetIndep->data + 1, event->data, event->length);
	packetIndep->len = 1 + event->length;
	
	releaseEvent(event);
	
	packetIndep->data[0] = 0;
	
	if(SDLNet_UDP_Send(UDPSocket, -1, packetIndep) == 0)
		printf("SDLNet_UDP_Send failed: %s", SDLNet_GetError());
}


bool Network::sendPacket(unsigned char frameTime) {
	// Send PING events when there are no other events to sustain connection
	if (packetOut->len == 1) {
		packetOut->len = 2;
		packetOut->data[0] = EVENT_PING;
	}
	
	packetOut->data[0] = frameTime;
	
	if(SDLNet_UDP_Send(UDPSocket, -1, packetOut) == 0) {
		printf("SDLNet_UDP_Send failed: %s", SDLNet_GetError());
		return false;
	}

	packetOut->len = 1;
	
	return true;
}


void Network::checkForData() {
	if(SDLNet_UDP_Recv(UDPSocket, packetIn)) {
		printf("Data received: %s \n", packetIn->data);
	}
}