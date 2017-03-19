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


MapData* map_create();
void map_free(MapData* map);

char* map_getInitData(MapData* map, int* dataLength, int* iconsCount);

void map_getSpawnPosition(MapData* map, float* x, float* y, int w, int h);
int map_collides(MapData* map, int x, int y, int w, int h);

#endif /* MAP_H */

