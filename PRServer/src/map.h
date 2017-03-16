#ifndef MAP_H
#define MAP_H

#define ICONS_AMOUNT 15
#define ICON_SIZE 32
#define ICON_STYLES_AMOUNT 7

typedef struct {
	int x;
	int y;
	char textureId;
} icon;

// TODO: map size
typedef struct {
	int width;
	int height;
	
	icon icons[ICONS_AMOUNT];
} MapData;


void map_initiate();
MapData* map_clone();
void map_free(MapData* map);

char* map_getInitData(int* dataLength, int* iconsCount);

#endif /* MAP_H */

