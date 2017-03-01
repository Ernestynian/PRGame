#include "Network.h"


Network::Network() {
	if (!init()
	 || !setServer(LOCALHOST, PORT)
	 || !createPackets(PACKET_SIZE))
		initialized = false;
	
	initialized = true;
	
	sendEvent(EVENT_CLIENT_JOIN, std::string(""));
}


Network::~Network() {
	if(initialized) {
		// Notify Server
		packetOut->len = 0;
		sendEvent(EVENT_CLIENT_EXIT, std::string(""));
		
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
	packetOut->len          = 0;
	
	
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
 * 
 * @return true on success
 */
void Network::addEvent(EventTypes eventType, const char* data, int length) {
	NetworkEvent* event = createEvent(eventType, data, length);
	
	if (packetOut->len + event->length < packetOut->maxlen) {
		memcpy(packetOut->data + packetOut->len, event->data, event->length);
		packetOut->len += event->length;
	}
	
	releaseEvent(event);
}


bool Network::sendEvent(EventTypes eventType, const std::string& str) {
	NetworkEvent* event = createEvent(eventType, str.c_str(), str.length());
	
	memcpy(packetIndep->data, event->data, event->length);
	packetIndep->len == event->length;
	
	releaseEvent(event);
	
	sendPacket();
	
	return true;
}


bool Network::sendPacket() {
	// Send PING events when there are no other events to sustain connection
	if (packetOut->len == 0) {
		packetOut->len = 1;
		packetOut->data[0] = EVENT_PING;
	}
	
	if(SDLNet_UDP_Send(UDPSocket, -1, packetOut) == 0) {
		printf("SDLNet_UDP_Send failed: %s", SDLNet_GetError());
		return false;
	}

	packetOut->len = 0;
	
	return true;
}


void Network::checkForData() {
	if(SDLNet_UDP_Recv(UDPSocket, packetIn)) {
		printf("Data received: %s \n", packetIn->data);
	}
}