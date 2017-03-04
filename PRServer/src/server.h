int  srv_start();
void srv_stop();
int  srv_transferPackets();
void srv_sendEventsToAll(unsigned char tick, unsigned char* eventsData, size_t dataLength);