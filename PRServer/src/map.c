#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "map.h"


MapData map = { 800 - ICON_SIZE, 600 - ICON_SIZE - 30 };
pthread_mutex_t mapMutex = PTHREAD_MUTEX_INITIALIZER;


int getRand(int limit) {
    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}


void map_initiate() {
	srand(time(NULL));
	
	// Make sure that ICONS_AMOUNT is low enough place every icon
	//  without colliding with each other
	for (int i = 0; i < ICONS_AMOUNT; ++i) {
		int collides = 1;
		while (collides) {
			map.icons[i].x = getRand(map.width);
			map.icons[i].y = getRand(map.height);
			collides = 0;
			for (int j = 0 ; j < i; ++j) {
				if (map.icons[i].x < map.icons[j].x + ICON_SIZE
				 && map.icons[i].x + ICON_SIZE > map.icons[j].x
				 && map.icons[i].y < map.icons[j].y + ICON_SIZE
				 && map.icons[i].y + ICON_SIZE > map.icons[j].y) {
					collides = 1;
					break;
				}					
			}
		}
		map.icons[i].textureId = getRand(ICON_STYLES_AMOUNT);
	}
}


MapData* map_clone() {
	MapData* mapClone = (MapData*)malloc(sizeof(MapData));
	
	pthread_mutex_lock(&mapMutex);
	*mapClone = map;
	pthread_mutex_unlock(&mapMutex);
	
	return mapClone;
}


void map_free(MapData* map) {
	free(map);
}


char* map_getInitData(int* dataLength, int* iconsCount) {
	*dataLength = 5 * ICONS_AMOUNT;
	*iconsCount = ICONS_AMOUNT;
	
	char* data = malloc(*dataLength);
	
	for (int i = 0; i < ICONS_AMOUNT; ++i) {
		char* _data = data + i * ICONS_AMOUNT;
		memcpy(_data + 0, &map.icons[i].x, 2);
		memcpy(_data + 2, &map.icons[i].y, 2);
		_data[4] = map.icons[i].textureId;
	}
	
	return data;
}