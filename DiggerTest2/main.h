#pragma once

#include <windows.h>

#define SYS_CYCLES 20
#define SYS_DEBUG 1

#define REN_HEADER_ROWS 1
#define REN_STATS_ROWS 2
#define REN_ROWS 20
#define REN_COLS 60

#define G_AREA_ROWS 40
#define G_AREA_COLS 120

#define BL_WALKABLE 1
#define BL_SLOWING 2
#define BL_BLOCKING 4

#define CHAR_EMPTY 32
#define CHAR_GRASS 44
#define CHAR_HEY_LOW 176
#define CHAR_HEY 177
#define CHAR_HEY_HIGH 178
#define CHAR_ROCK 254
#define CHAR_FRAME_UL    // Frame char of upper left corner
#define CHAR_FRAME_UR    // Frame char of upper right corner
#define CHAR_FRAME_UPPER // Frame char of upper horizontal line
#define CHAR_FRAME_LL    // Frame char of lower left corner
#define CHAR_FRAME_LR    // Frame char of lower right corner
#define CHAR_FRAME_LOWER // Frame char of lower horizontal line
#define CHAR_FRAME_L     // Frame char of left vertical line
#define CHAR_FRAME_R     // Frame char of right vertical line



typedef struct system {
	char debug;
	char show_fps;
	char show_frames;
} System;


typedef struct block {
	unsigned char tile;
	int type;
} Block;

Block blocks[] = {
	{ CHAR_EMPTY, BL_WALKABLE },
	{ CHAR_GRASS, BL_WALKABLE },
	{ CHAR_HEY_LOW, BL_WALKABLE },
	{ CHAR_HEY, BL_SLOWING },
	{ CHAR_HEY_HIGH, BL_SLOWING },
	{ CHAR_ROCK, BL_BLOCKING },
};



typedef struct map {
	Block block;
	const char desc;
	const char tag;
} Map;

Map map[G_AREA_ROWS][G_AREA_COLS];


typedef struct player {
	unsigned char name; // Player name
	COORD Position;     // Player current position
	COORD PositionOld;  // Player old position
	unsigned char dir;    // Player direction
	unsigned char marker; // Player "icon"

	unsigned int level; // Player level
	unsigned int exp;   // Player experience
	int health;         // Player health

	int atkbase;  // Attack points
	int atkboost; // Attack boost points
	int defbase;  // Defence points
	int defboost; // Defence boost points
} Player;

/* System variables
0: COORD header upper left
1: COORD header lower right
2: COORD game area upper left
3: COORD game area lower right
4: COORD stats area upper left
5: COORD stats area lower left
6: Toggle if header is updated
7: Toggle if game area is updated
8: Toggle if stats area is updated
*/
typedef struct renderer {
	COORD header_start;
	COORD header_end;
	COORD gamearea_start;
	COORD gamearea_end;
	COORD statsarea_start;
	COORD statsarea_end;
	char upd_header;
	char upd_gamearea;
	char upd_stats;
} Renderer;

