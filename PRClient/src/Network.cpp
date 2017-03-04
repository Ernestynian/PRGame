#include "Network.h"

#include "../../Common/byteConverter.h"

Network::Network() {
	if (!init()
	 || !setServer(LOCALHOST, PORT)
	 || !createPackets())
		initialized = false;
	
	initialized = true;
	
	sendEvent(NET_EVENT_CLIENT_JOIN, 0, 0);
}


Network::~Network() {
	if(initialized) {
		// Notify Server
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


bool Network::createPackets() {
	packetIn = SDLNet_AllocPacket(MAX_SERVER_PACKET_SIZE);

	if(packetIn == nullptr) {
		printf("SDLNet_AllocPacket failed: %s", SDLNet_GetError());
		return false;
	}
	
	
	packetOut = SDLNet_AllocPacket(MAX_CLIENT_PACKET_SIZE);

	if(packetOut == nullptr) {
		printf("SDLNet_AllocPacket failed: %s", SDLNet_GetError());
		return false;
	}

	packetOut->address.host = serverAddress.host;
	packetOut->address.port = serverAddress.port;
	packetOut->len          = 1;
	
	
	packetDirect = SDLNet_AllocPacket(MAX_CLIENT_PACKET_SIZE);

	if(packetDirect == nullptr) {
		printf("SDLNet_AllocPacket failed: %s", SDLNet_GetError());
		return false;
	}
	
	packetDirect->address.host = serverAddress.host;
	packetDirect->address.port = serverAddress.port;
	
	return true;
}


bool Network::recieviedAcceptMessage() {
	checkForData();
	
	if (packetIn->data[1] == NET_EVENT_CLIENT_ACCEPTED) {
		printf("NET_EVENT_CLIENT_ACCEPTED\n");
		return true;
	}
	
	return false;
}


/**
 * Append a new event to the packet
 * Format and arguments are byteConverter.h
 * Look in there for more informations
 */
void Network::addNewEvent(EventTypes eventType, const char* format, ...) {
	va_list args;
    va_start(args, format);
	
	int size;
	char* bytes = toBytes(format, &size, args);
	NetworkEvent* event = createEvent(eventType, bytes, size);
	
	if (packetOut->len + event->length < packetOut->maxlen) {
		memcpy(packetOut->data + packetOut->len, event->data, event->length);
		packetOut->len += event->length;
	}
	
	releaseEvent(event);
	free(bytes);
	
	va_end(args);
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


/**
 * Packet consist of multiple events and an ID.
 * @param frameTime - also packet ID, newer must be higher (or 0)
 * @return true on success
 */
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
	currentEvent = packetIn->data + 1;
}


EventTypes Network::getNextEvent() {
	currentEvent += currentEvent[1];
	
	return (EventTypes)*currentEvent;
}


uint8_t* Network::getCurrentEventData() {
	return currentEvent + 2;
}