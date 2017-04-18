#include "Network.h"

#include "../../Common/byteConverter.h"

Network::Network() {
	if (!init()
	 || !setServer(LOCALHOST, PORT)
	 || !createPackets())
		initialized = false;
	
	initialized = true;
	
	previousServerTick = 0;
	
	sendEvent(NET_EVENT_CLIENT_JOIN, 0, 0);
}


Network::~Network() {
	if(initialized) {
		// Notify Server
		printf("Closing network...\n");
		sendEvent(NET_EVENT_CLIENT_EXIT, 0, 0);
		
		// Clean up
		SDLNet_FreePacket(packetOut);
		SDLNet_FreePacket(packetIn);
		SDLNet_FreePacket(packetDirect);
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

void Network::packetSendingThread(unsigned int tickrate) {
    int last = SDL_GetTicks();
    for(;;) {
        frame++;
        if (frame == 0)
                frame++;
        
        sendPacket(frame);
        
        //ticking
        int delta = SDL_GetTicks() - last;
        SDL_Delay((int)(1000.0/tickrate) - delta);
        last = SDL_GetTicks();
    }
}

void Network::start(unsigned int tickrate) {
    sendingThread = std::thread(&Network::packetSendingThread, this, tickrate);
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


bool Network::receivedAcceptMessage() {
	if (receivePacket()) {
		if (packetIn->data[1] == NET_EVENT_CLIENT_ACCEPTED) {
			printf("NET_EVENT_CLIENT_ACCEPTED\n");
			return true;
		}
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
	char* bytes = toBytes(&size, format, args);
	NetworkEvent* event = createEvent(eventType, bytes, size);
	
	if (packetOut->len + event->length < packetOut->maxlen) {
		memcpy(packetOut->data + packetOut->len, event->data, event->length);
		packetOut->len += event->length;
	}
	
	releaseEvent(event);
	free(bytes);
	
	va_end(args);
}


void Network::addNewEvent(EventTypes eventType) {
	NetworkEvent* event = createEvent(eventType, 0, 0);
	
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


/**
 * Receive a single packet from the UDP socket.
 * @return whether new packet was received
 */
bool Network::receivePacket() {
	int status = SDLNet_UDP_Recv(UDPSocket, packetIn);
	
	if (status > 0) {
		if (isPacketNewer(packetIn->data[0], &previousServerTick))
			currentEvent = packetIn->data + 1;
		else
			status = 0;
	}
	
	return status > 0;
}


bool Network::isThereMoreEvents() {
	if (packetIn->len < 3)
		return false;
	
	int currentPosition = currentEvent - packetIn->data;
	// + 1 is a tick byte and another + 1 a "next" event type
	int lastPosition    = packetIn->len - (getCurrentEventDataLength() + 1 + 1);
	return currentPosition < lastPosition;
}

EventTypes Network::getNextEvent() {
	currentEvent += getCurrentEventDataLength() + 2;
	
	return getCurrentEventDataType();
}


EventTypes Network::getCurrentEventDataType() {
	initBinaryReader((char*)getCurrentEventData());
	return (EventTypes)*currentEvent;
}


char Network::getCurrentEventDataLength() {
	return currentEvent[1];
}


uint8_t* Network::getCurrentEventData() {
	return currentEvent + 2;
}