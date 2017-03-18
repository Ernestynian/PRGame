#ifndef MAP_H
#define MAP_H

#define ICONS_AMOUNT 15
#define ICON_SIZE 32
#define ICON_STYLES_AMOUNT 19

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
char* map_getInitData(int* dataLength, int* iconsCount);

MapData* map_clone();
void map_free(MapData* map);

void map_getSpawnPosition(MapData* map, float* x, float* y, int w, int h);
int map_collides(MapData* map, int x, int y, int w, int h);

#endif /* MAP_H */

