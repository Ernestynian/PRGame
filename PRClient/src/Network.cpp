#include "Network.h"


Network::Network() {
	if (!init()
	 || !setServer(LOCALHOST, PORT)
	 || !createPackets(MAX_PACKET_SIZE))
		initialized = false;
	
	initialized = true;
	
	sendEvent(NET_EVENT_CLIENT_JOIN, 0, 0);
}


Network::~Network() {
	if(initialized) {
		// Notify Server
		packetOut->len = 0;
		sendEvent(NET_EVENT_CLIENT_EXIT, 0, 0);
		
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
	
	packetType = packetIn->data + 1;
	
	
	packetOut = SDLNet_AllocPacket(packetSize);

	if(packetOut == nullptr) {
		printf("SDLNet_AllocPacket failed: %s", SDLNet_GetError());
		return false;
	}

	packetOut->address.host = serverAddress.host;
	packetOut->address.port = serverAddress.port;
	packetOut->len          = 1;
	
	
	packetDirect = SDLNet_AllocPacket(packetSize);

	if(packetDirect == nullptr) {
		printf("SDLNet_AllocPacket failed: %s", SDLNet_GetError());
		return false;
	}
	
	packetDirect->address.host = serverAddress.host;
	packetDirect->address.port = serverAddress.port;
	
	return true;
}


bool Network::checkIfHasBeenAccepted() {
	checkForData();
	
	if (*packetType == NET_EVENT_CLIENT_ACCEPTED) {
		printf("NET_EVENT_CLIENT_ACCEPTED\n");
		return true;
	}
	
	return false;
}


/**
 * Append a new event to the packet
 */
void Network::addNewEvent(EventTypes eventType, const char* data, int length) {
	NetworkEvent* event = createEvent(eventType, data, length);
	
	if (packetOut->len + event->length < packetOut->maxlen) {
		memcpy(packetOut->data + packetOut->len, event->data, event->length);
		packetOut->len += event->length;
	}
	
	releaseEvent(event);
}


void Network::sendEvent(EventTypes eventType, const char* data, int length) {
	NetworkEvent* event = createEvent(eventType, data, length);
	
	memcpy(packetDirect->data + 1, event->data, event->length);
	packetDirect->len = 1 + event->length;
	
	releaseEvent(event);
	
	packetDirect->data[0] = 0;
	
	if(SDLNet_UDP_Send(UDPSocket, -1, packetDirect) == 0)
		printf("SDLNet_UDP_Send failed: %s", SDLNet_GetError());
}


bool Network::sendPacket(unsigned char frameTime) {
	// Send PING events when there are no other events to sustain connection
	if (packetOut->len == 1) {
		packetOut->len = 2;
		packetOut->data[1] = NET_EVENT_PING;
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
	SDLNet_UDP_Recv(UDPSocket, packetIn);
}