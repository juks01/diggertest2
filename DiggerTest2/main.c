#include "main.h"
#include "tools.h"
#include <stdio.h>
#include <time.h>


static void gen_gamearea(Renderer* rend) {
	COORD start = rend->gamearea_start;
	COORD end = rend->gamearea_end;
	COORD pos;
	int num_blocks = sizeof(blocks) / sizeof(blocks[0]);
	for (pos.Y = 0; pos.Y < end.Y; pos.Y++)
		for (pos.X = 0; pos.X < end.X; pos.X++)
			gamemap[pos.Y][pos.X].block = blocks[random_number(0, num_blocks)];
}

static void gen_rendarea(Renderer* rend) {
	COORD start = rend->rendarea_start;
	COORD end = rend->rendarea_end;
	COORD pos;
	unsigned char header[REN_COLS] = "---=== HEADER ROW ===---";

	for (pos.Y = 0; pos.Y < end.Y; pos.Y++) {
		for (pos.X = 0; pos.X < end.X; pos.X++)
			// == Header row
			if (pos.Y == 0)
				rendmap[pos.Y][pos.X].block.tile = header[pos.X];

			// == First row of gamearea "box"
			else if (pos.Y == start.Y) {
				if (pos.X == start.X)        // Upper left corner
					rendmap[pos.Y][pos.X].block = walls[0];
				else if (pos.X == end.X - 1) // Upper right corner
					rendmap[pos.Y][pos.X].block = walls[1];
				else                     // Upper center characters
					rendmap[pos.Y][pos.X].block = walls[2];
			}
			// == Last row of gamearea "box"
			else if (pos.Y == end.Y - 1) {
				if (pos.X == start.X)        // Lower left corner
					rendmap[pos.Y][pos.X].block = walls[3];
				else if (pos.X == end.X - 1) // Lower right corner
					rendmap[pos.Y][pos.X].block = walls[4];
				else                     // Lower center characters
					rendmap[pos.Y][pos.X].block = walls[5];
			}
			// == Rows in the middle
			else {                       
				if (pos.X == start.X)        // First column
					rendmap[pos.Y][pos.X].block = walls[6];
				else if (pos.X == end.X - 1) // Last column
					rendmap[pos.Y][pos.X].block = walls[7];
				else // Game area between borders - leave empty for real gamemap
					rendmap[pos.Y][pos.X].block = blocks[0];
			}
	}
}

static int draw_gamearea(Renderer* rend, HANDLE* hOut) {
	COORD end = rend->rendarea_end;

	// Save current attributes
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;
	GetConsoleScreenBufferInfo(hOut, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	COORD pos;
	for (pos.Y = 2; pos.Y < end.Y-1; pos.Y++) {
		for (pos.X = 1; pos.X < end.X-1; pos.X++) {
			SetConsoleCursorPosition(hOut, pos);
			SetConsoleTextAttribute(hOut, gamemap[pos.Y][pos.X].block.color);
			printf("%c", gamemap[pos.Y][pos.X].block.tile);
			SetConsoleTextAttribute(hOut, saved_attributes);
		}
	}
	return 0;
}

static int draw_rendarea(Renderer* rend, HANDLE* hOut) {
	COORD start = rend->rendarea_start;
	COORD end = rend->rendarea_end;

	COORD pos;
	for (pos.Y = 0; pos.Y < end.Y; pos.Y++) {
		for (pos.X = 0; pos.X < end.X; pos.X++) {
			SetConsoleCursorPosition(hOut, pos);

			// Draw the game area box
			if (pos.Y == start.Y || pos.Y == end.Y ||
				pos.X == start.X || pos.X == pos.X)
				printf("%c", rendmap[pos.Y][pos.X].block.tile);
		}
	}
	return 0;
}



static void moveplayer(Renderer* rend, Player* plr, HANDLE hOut, int key) {
	unsigned char marker = "@";
	int posx = plr->Position.X;
	int posy = plr->Position.Y;

	plr->dir = key;

	switch (plr->dir) {
	case 1: { // Trying to go up
		if (posy > 2 &&
			gamemap[posy-1][posx].block.type != BL_BLOCKING) {
			plr->PositionOld = plr->Position;
			plr->Position.Y--;
			marker = "^";
		}
		break;
	}
	case 2: { // Trying to go right
		if (posx < rend->rendarea_end.X-2 &&
			gamemap[posy][posx+1].block.type != BL_BLOCKING) {
			plr->PositionOld = plr->Position;
			plr->Position.X++;
			marker = ">";
		}
		break;
	}
	case 3: { // Trying to go down
		if (posy < rend->rendarea_end.Y-2 &&
			gamemap[posy + 1][posx].block.type != BL_BLOCKING) {
			plr->PositionOld = plr->Position;
			plr->Position.Y++;
			marker = "v";
		}
		break;
	}
	case 4: { // Trying to go left
		if (posx > 1 &&
			gamemap[posy][posx - 1].block.type != BL_BLOCKING) {
			plr->PositionOld = plr->Position;
			plr->Position.X--;
			marker = "<";
		}
		break;
	}
	default: printf("\n UNKNOWN DIRECTION ");
	}

	// Update player info
	plr->marker = marker;

	// Print player marker to map
	SetConsoleCursorPosition(hOut, plr->Position);
	printf("%c", marker);

	// Update character in player old position from map

	// Save current attributes
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;
	GetConsoleScreenBufferInfo(hOut, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	if (!(plr->Position.Y == plr->PositionOld.Y &&
		plr->Position.X == plr->PositionOld.X)) {
		SetConsoleCursorPosition(hOut, plr->PositionOld);
		SetConsoleTextAttribute(hOut, gamemap[plr->PositionOld.Y][plr->PositionOld.X].block.color);
		printf("%c", gamemap[plr->PositionOld.Y][plr->PositionOld.X].block.tile);
		SetConsoleTextAttribute(hOut, saved_attributes);
	}
}



static int init(System* sys, Renderer* rend, Player* plr, HANDLE* hOut) {
	srand((unsigned)time(0));
	
	sys->debug = SYS_DEBUG;
	sys->show_fps = 1;
	sys->show_frames = 1;

	rend->rendarea_start.X = 0;
	rend->rendarea_start.Y = REN_HEADER_ROWS;
	rend->rendarea_end.X = REN_COLS;
	rend->rendarea_end.Y = REN_HEADER_ROWS + REN_ROWS;
	rend->header_start.X = 0;
	rend->header_start.Y = 0;
	rend->header_end.X = REN_COLS;
	rend->header_end.Y = REN_HEADER_ROWS;
	rend->gamearea_start.X = 2;
	rend->gamearea_start.Y = 2;
	rend->gamearea_end.X = G_AREA_COLS;
	rend->gamearea_end.Y = G_AREA_ROWS;
	rend->statsarea_start.X = 0;
	rend->statsarea_start.Y = REN_HEADER_ROWS + REN_ROWS;
	rend->statsarea_end.X = REN_COLS;
	rend->statsarea_end.Y = REN_HEADER_ROWS + REN_ROWS + REN_STATS_ROWS;
	rend->upd_rendarea = 1;
	rend->upd_header = 1;
	rend->upd_stats = 1;

	plr->name = "Player";
	plr->marker = "@";
	plr->dir = 9;
	plr->Position.X = 10;
	plr->Position.Y = 10;
	plr->PositionOld = plr->Position;
	plr->level = 1;
	plr->exp = 0;
	plr->exp_next = plr->level*200;
	plr->atkbase = 1 + plr->level;
	plr->atkboost = 0;
	plr->defbase = 1 + plr->level;
	plr->defboost = 0;
	plr->health_max = 10 + plr->level + plr->atkbase + plr->defbase;
	plr->health = plr->health_max;

	// Hide cursor
	CONSOLE_CURSOR_INFO cur;
	cur.dwSize = 100;
	cur.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &cur);

	// Generate random map
	gen_gamearea(rend);
	// Generate rend area
	gen_rendarea(rend);

	return 0;
}

static char input() {
	if (_kbhit())
		return _getch();
}



static int update(System* sys, Renderer* rend, Map* map, Player* plr, HANDLE hOut) {
	if (plr->exp >= plr->exp_next) {
		plr->level++;
		plr->exp_next = plr->level*plr->level * 200;
	}
	plr->atkbase = 1 + plr->level;
	plr->defbase = 1 + plr->level;
	plr->health_max = 10 + plr->level + plr->atkbase + plr->defbase;
	if (rend->upd_stats)
		rend->upd_stats = rend_upd_stats(sys, rend, plr, hOut);
	return 0;
}


void draw_debug(System* sys, Renderer* rend, Map* map, Player* plr, HANDLE hOut) {
	COORD posD1, posD2, posD3;
	posD1.Y = 0;
	posD1.X = 40;
	posD2.Y = REN_ROWS + 1;
	posD2.X = 49;
	posD3.Y = REN_ROWS + 2;
	posD3.X = 49;
	SetConsoleCursorPosition(hOut, posD1);
	printf("Pos: %d.%d", plr->Position.Y, plr->Position.X);
}


static int renderer(System* sys, Renderer* rend, Map* map, Player* plr, HANDLE hOut) {
	if (rend->upd_rendarea)
		rend->upd_rendarea = draw_rendarea(rend, hOut);
	if (rend->upd_gamearea)
		rend->upd_gamearea = draw_gamearea(rend, hOut);
	if (rend->upd_debug)
		draw_debug(sys, rend, map, plr, hOut);
	return 0;
}



static int rend_upd_stats(System* sys, Renderer* rend, Player* plr, HANDLE* hOut) {
	if (sys->debug) { // Debug texts
		COORD cd = rend->statsarea_start;
		cd.X += 49;
		SetConsoleCursorPosition(hOut, cd);
		printf("S-DEBUG: 12345678901234567890");
		cd.Y++;
		SetConsoleCursorPosition(hOut, cd);
		printf("S-DEBUG: 12345678901234567890");
	}

	SetConsoleCursorPosition(hOut, rend->statsarea_start);
	int atk = plr->atkbase + plr->atkboost; // Total Atk
	int def = plr->defbase + plr->defboost; // Total Def
	printf(" HP: %d/%d          Lvl: %d   Exp: %d/%d\n",
		plr->health, plr->health_max, plr->level, plr->exp, plr->exp_next);
	printf("Atk: %d   Def: %d", atk, def);

	return 0;
}




int main(int argc, char* argv[]) {

	// Initialization
	System sys;
	System* psys = &sys;
	Renderer rend;
	Renderer* prend = &rend;
	Map m;
	Map* pmap = &m;
	Player plr;
	Player* pplr = &plr;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	int ret_init = 0, ret_update = 0, ret_renderer = 0;
	ret_init = init(psys, prend, pplr, hOut);


	// THE MAIN LOOP
	int cycles = 0;
	while (1) {
		sleep_ms(SYS_CYCLES);
		if (cycles < 5000)
			cycles++;
		else
			cycles = 0;


		//input
		switch (input()) {
		case 'w': moveplayer(prend, pplr, hOut, 1);  break;
		case 'a': moveplayer(prend, pplr, hOut, 4);  break;
		case 's': moveplayer(prend, pplr, hOut, 3);  break;
		case 'd': moveplayer(prend, pplr, hOut, 2);  break;
		case 'q': exit(0); break;
		
		// Just for testing...
		case 'k':
			plr.exp += 100;
			rend.upd_stats = 1;
			break;
		case 'l':
			plr.level++;
			rend.upd_stats = 1;
			break;
		}


		//update
		ret_update = update(psys, prend, pmap, pplr, hOut);


		//renderer
		ret_renderer = renderer(psys, prend, pmap, pplr, hOut);


	} // END OF MAIN LOOP
	return 0;
}
