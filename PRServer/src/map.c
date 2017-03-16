#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "map.h"

#define RESERVED_HEIGHT 90
MapData map = { 800 - ICON_SIZE, 600 - RESERVED_HEIGHT };
pthread_mutex_t mapMutex = PTHREAD_MUTEX_INITIALIZER;


int getRand(int limit) {
    int divisor = RAND_MAX / (limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}


void map_initiate() {
	srand(time(NULL));
	
	int horizontalSpaces = map.height / ICON_SIZE - 2;
	
	char* usedHeights = malloc(horizontalSpaces);
	memset(usedHeights, 0, horizontalSpaces);
	
	char horizontalBoxId = horizontalSpaces;
	int currentHeight = (horizontalBoxId + 2) * ICON_SIZE;
	usedHeights[horizontalBoxId] = 1;
	
	int currentHeightIcons = 1;
	
	for (int i = 0; i < ICONS_AMOUNT; ++i) {
		if (getRand(100) < 50 + currentHeightIcons * 3) {
			// Prevent infinite loop with range (1, horizontalSpaces)
			int j;
			for (j = 0; j < horizontalSpaces; ++j) {
				horizontalBoxId = getRand(horizontalSpaces);
				
				if (usedHeights[horizontalBoxId] == 0) {
					if (horizontalBoxId != 0)
						usedHeights[horizontalBoxId - 1] = 2;
					usedHeights[horizontalBoxId] = 1;
					usedHeights[horizontalBoxId + 1] = 2;
					break;
				}
			}
			
			// Check if no slots are available
			if (j == horizontalSpaces) {
				// Pick random used
				do {
					horizontalBoxId = getRand(horizontalSpaces);
				} while (usedHeights[horizontalBoxId] != 1);
			}
			
			currentHeight = (horizontalBoxId + 2) * ICON_SIZE;
			currentHeightIcons = 1;
		} else 
			currentHeightIcons++;
		map.icons[i].y = currentHeight;
		
		int collides = 1;
		while (collides) {
			map.icons[i].x = getRand(map.width / ICON_SIZE) * ICON_SIZE;
			collides = 0;
			for (int j = 0 ; j < i; ++j) {
				// TODO: don't allow the same x only on the same y and y + 1 and y - 1
				if (map.icons[i].x < map.icons[j].x + ICON_SIZE
				 && map.icons[i].x + ICON_SIZE > map.icons[j].x
				 && map.icons[i].y < map.icons[j].y + ICON_SIZE * 3
				 && map.icons[i].y + ICON_SIZE * 3 > map.icons[j].y) {
					collides = 1;
					break;
				}					
			}
		}
		map.icons[i].textureId = getRand(ICON_STYLES_AMOUNT);
	}
	
	free(usedHeights);
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
		char* _data = data + i * 5;
		memcpy(_data + 0, &map.icons[i].x, 2);
		memcpy(_data + 2, &map.icons[i].y, 2);
		_data[4] = map.icons[i].textureId;
	}
	
	return data;
}