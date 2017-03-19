#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "map.h"

#define RESERVED_HEIGHT 90


int getRand(int limit) {
    int divisor = RAND_MAX / (limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}


MapData* map_create() {
	srand(time(NULL));
	
	MapData* map = malloc(sizeof(MapData));
	map->width = 800 - ICON_SIZE;
	map->height = 600 - RESERVED_HEIGHT;
	
	int horizontalSpaces = map->height / ICON_SIZE - 2;
	
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
		map->icons[i].y = currentHeight;
		
		int collides = 1;
		while (collides) {
			map->icons[i].x = getRand(map->width / ICON_SIZE) * ICON_SIZE;
			collides = 0;
			for (int j = 0 ; j < i; ++j) {
				if (map->icons[i].x < map->icons[j].x + ICON_SIZE
				 && map->icons[i].x > map->icons[j].x - ICON_SIZE
				 && map->icons[i].y < map->icons[j].y + ICON_SIZE * 3
				 && map->icons[i].y > map->icons[j].y - ICON_SIZE * 3) {
					collides = 1;
					break;
				}					
			}
		}
		map->icons[i].textureId = getRand(ICON_STYLES_AMOUNT);
	}
	
	free(usedHeights);
	return map;
}


void map_free(MapData* map) {
	free(map);
}


char* map_getInitData(MapData* map, int* dataLength, int* iconsCount) {
	*dataLength = 5 * ICONS_AMOUNT;
	*iconsCount = ICONS_AMOUNT;
	
	char* data = malloc(*dataLength);
	
	for (int i = 0; i < ICONS_AMOUNT; ++i) {
		char* _data = data + i * 5;
		memcpy(_data + 0, &map->icons[i].x, 2);
		memcpy(_data + 2, &map->icons[i].y, 2);
		_data[4] = map->icons[i].textureId;
	}
	
	return data;
}


void map_getSpawnPosition(MapData* map, float* x, float* y, int w, int h) {
	int collides = 1;
	while(collides) {
		*x = getRand(map->width  - w);
		*y = getRand(map->height - h);
		
		collides = map_collides(map, *x, *y, w, h);
	}
}


int map_collides(MapData* map, int x, int y, int w, int h) {
	for (int i = 0; i < ICONS_AMOUNT; ++i) {
		if (map->icons[i].x             < x + w
		 && map->icons[i].x + ICON_SIZE > x 
		 && map->icons[i].y             < y + h
		 && map->icons[i].y + ICON_SIZE > y)
			return 1;
	}
	
	return 0;
}