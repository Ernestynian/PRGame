#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include "byteConverter.h"

char* buffer;
int   pos;

void convertInt(int a) {
	buffer[pos + 0] =  a        & 0xFF;
	buffer[pos + 1] = (a >> 8)  & 0xFF;
	buffer[pos + 2] = (a >> 16) & 0xFF;
	buffer[pos + 3] = (a >> 24) & 0xFF;
	
	pos += sizeof(a);
}

void convertShort(short a) {
	buffer[pos + 0] =  a        & 0xFF;
	buffer[pos + 1] = (a >> 8)  & 0xFF;
	
	pos += sizeof(a);
}

void convertByte(char a) {
	buffer[pos] =  a;
	
	pos += sizeof(a);
}

///////////////
// INTERFACE //
///////////////

char* toBytes(int* bytesCount, const char* types, va_list valist) {
	pos = 0;
	
	int args = strlen(types);
	
	int size = 0;
	for (int i = 0; i < args; i++)
		size += types[i] - '0';
	
	buffer = malloc(size);
	
	for (int i = 0; i < args; i++) {
		switch (types[i]) {
			case '4':
				convertInt(va_arg(valist, int32_t));
				break;
			case '2':
				convertShort(va_arg(valist, int32_t));
				break;
			case '1':
				convertByte(va_arg(valist, int32_t));
				break;
		}
	}
	
	*bytesCount = pos;
	
	return buffer;
}


char* toBytesV(int* bytesCount, const char* types, ...) {
	va_list valist;
	va_start(valist, types);
	
	char* buffer = toBytes(bytesCount, types, valist);
	
	va_end(valist);
	
	return buffer;
}


const uint8_t* reader_bytes;
int reader_position;

void initBinaryReader(const char* bytes) {
	reader_bytes = bytes;
	reader_position = 0;
}


int32_t binaryRead4B() {
	int32_t var;
	
	var = reader_bytes[reader_position] + 
		 (reader_bytes[reader_position + 1] << 8)  +
		 (reader_bytes[reader_position + 2] << 16) +
		 (reader_bytes[reader_position + 3] << 24);
	
	reader_position += sizeof(int32_t);
	return var;
}

int16_t binaryRead2B() {
	int16_t var;
	
	var = reader_bytes[reader_position] +
		 (reader_bytes[reader_position + 1] << 8);

	
	reader_position += sizeof(int16_t);
	return var;
}

int8_t binaryRead1B() {
	return reader_bytes[reader_position++];
}