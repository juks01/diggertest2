#pragma once

#ifdef _WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#include <ncurses.h> // for cursot positioning etc
typedef short SHORT;
typedef struct _COORD {
	SHORT X;
	SHORT Y;
} COORD, * PCOORD;
#endif

#define SYS_CYCLES 20
#define SYS_DEBUG 1

#define REN_HEADER_ROWS 1
#define REN_STATS_ROWS 2
#define REN_ROWS 22
#define REN_COLS 78

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
#define CHAR_SOLID 219
#define CHAR_FRAME_UL 218   // Frame char of upper left corner
//#define CHAR_FRAME_UR 183   // Frame char of upper right corner
#define CHAR_FRAME_UR 44   // Frame char of upper right corner
#define CHAR_FRAME_UC 196   // Frame char of upper horizontal line
//#define CHAR_FRAME_LL 212   // Frame char of lower left corner
#define CHAR_FRAME_LL 96   // Frame char of lower left corner
#define CHAR_FRAME_LR 188   // Frame char of lower right corner
#define CHAR_FRAME_LC 205   // Frame char of lower horizontal line
#define CHAR_FRAME_L  179   // Frame char of left vertical line
#define CHAR_FRAME_R  186   // Frame char of right vertical line




typedef struct system {
	char debug;
	char show_fps;
	char show_frames;
} System;


typedef struct block {
	unsigned char tile;
	int type;
	char color;
} Block;

Block blocks[] = {
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_EMPTY, BL_WALKABLE, 0x0 },
	{ CHAR_GRASS, BL_WALKABLE, FOREGROUND_GREEN },
	{ CHAR_GRASS, BL_WALKABLE, FOREGROUND_GREEN },
	{ CHAR_GRASS, BL_WALKABLE, FOREGROUND_GREEN },
	{ CHAR_HEY_LOW, BL_WALKABLE, FOREGROUND_GREEN },
	{ CHAR_HEY, BL_SLOWING, FOREGROUND_GREEN },
	{ CHAR_HEY_HIGH, BL_SLOWING, FOREGROUND_GREEN },
	{ CHAR_ROCK, BL_BLOCKING, FOREGROUND_INTENSITY },
	{ CHAR_SOLID, BL_BLOCKING, FOREGROUND_INTENSITY }
};

/* Wall chars
0: UL
1: UR
2: UC
3: LL
4: LR
5: LC
6: L
7: R
*/
Block walls[] = {
	{ CHAR_FRAME_UL, BL_BLOCKING },
	{ CHAR_FRAME_UR, BL_BLOCKING },
	{ CHAR_FRAME_UC, BL_BLOCKING },
	{ CHAR_FRAME_LL, BL_BLOCKING },
	{ CHAR_FRAME_LR, BL_BLOCKING },
	{ CHAR_FRAME_LC, BL_BLOCKING },
	{ CHAR_FRAME_L, BL_BLOCKING },
	{ CHAR_FRAME_R, BL_BLOCKING },
};



typedef struct map {
	Block block;
	const char desc;
	const char tag;
} Map;

Map gamemap[G_AREA_ROWS][G_AREA_COLS];
Map rendmap[REN_ROWS][REN_COLS];


typedef struct player {
	unsigned char name;    // Player name
	COORD Position;        // Player current position
	COORD PositionOld;     // Player old position
	unsigned char dir;     // Player direction
	unsigned char marker;  // Player "icon"

	unsigned int level;    // Player level
	unsigned int exp;      // Player experience
	unsigned int exp_next; // Player exp to next level
	int health;            // Player health
	int health_max;        // Player max health

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
7: Toggle if stats area is updated
*/
typedef struct renderer {
	COORD rendarea_start;
	COORD rendarea_end;
	COORD header_start;
	COORD header_end;
	COORD gamearea_start;
	COORD gamearea_end;
	COORD gamearea_current_start;
	COORD statsarea_start;
	COORD statsarea_end;
	char upd_rendarea;
	char upd_gamearea;
	char upd_header;
	char upd_stats;
	char upd_debug;
} Renderer;


