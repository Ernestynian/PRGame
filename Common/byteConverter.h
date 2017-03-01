#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convert parameters to array of bytes that can be send over network
 * @param Each number is one variable, allowed numbers are: 1, 2, 3
 * @param ...
 * @return Array of bytes
 */
char* toBytes(const char* types, ...);

void    initBinaryReader(const char* bytes);
int32_t binaryRead4B();
int16_t binaryRead2B();
int8_t  binaryRead1B();

#ifdef __cplusplus
}
#endif