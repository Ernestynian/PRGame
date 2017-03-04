#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convert parameters to array of bytes that can be send over network
 * @param Each number is one variable, allowed numbers are: 1, 2, 3
 * @param Out variable that returns calculated size of bytes
 * @param List of parameters
 * @return Array of bytes that needs to be freed
 */
char*   toBytes(const char* types, int* bytesCount, va_list valist);
/**
 * Convert parameters to array of bytes that can be send over network
 * @param Each number is one variable, allowed numbers are: 1, 2, 3
 * @param Out variable that returns calculated size of bytes
 * @param ...
 * @return Array of bytes that needs to be freed
 */
char*   toBytesV(const char* types, int* bytesCount, ...);

/**
 * Reset binaryRead functions
 * @param Array of bytes to be read from
 */
void    initBinaryReader(const char* bytes);
int32_t binaryRead4B();
int16_t binaryRead2B();
int8_t  binaryRead1B();

#ifdef __cplusplus
}
#endif