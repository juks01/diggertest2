#include "main.h"
#include "tools.h"
#include <conio.h>
#include <stdio.h>
#include <time.h>

static int rend_upd_header(System* sys, Renderer* rend, Map* map, HANDLE hOut) {
	SetConsoleCursorPosition(hOut, rend->header_start);
	printf(" ---=== HEADER ROW ===--- ");
	if (sys->debug) { // Debug texts
		COORD start = rend->header_start;
		start.X += 49;
		SetConsoleCursorPosition(hOut, start);
		printf("H-DEBUG: 12345678901234567890");
	}
	return 0;
}


static void gen_rendarea(Renderer* rend) {
	COORD start = rend->rendarea_start;
	COORD end = rend->rendarea_end;
	int num_blocks = sizeof(blocks) / sizeof(blocks[0]);
	for (int r = start.Y; r < end.Y; r++)
		for (int c = start.X; c < end.X; c++)
			if (r == start.Y) {          // == The first row ==
				if (c == start.X)        // Upper left corner
					rendmap[r][c].block = walls[0];
				else if (c == end.X - 1) // Upper right corner
					rendmap[r][c].block = walls[1];
				else                     // Upper center characters
					rendmap[r][c].block = walls[2];
			}
			else if (r == end.Y - 1) {   // == The last row ==
				if (c == start.X)        // Lower left corner
					rendmap[r][c].block = walls[3];
				else if (c == end.X - 1) // Lower right corner
					rendmap[r][c].block = walls[4];
				else                     // Lower center characters
					rendmap[r][c].block = walls[5];
			}
			else {                       // == Rows in the middle
				if (c == start.X)        // First column
					rendmap[r][c].block = walls[6];
				else if (c == end.X - 1) // Last column
					rendmap[r][c].block = walls[7];
				else // Game area between borders
					gamemap[r][c].block = blocks[random_number(0, num_blocks)];
			}
}



static int draw_rendarea(Renderer* rend, HANDLE* hOut) {
	COORD start = rend->rendarea_start;
	COORD end = rend->rendarea_end;
	
	SetConsoleCursorPosition(hOut, start);

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;
	/* Save current attributes */
	GetConsoleScreenBufferInfo(hOut, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	COORD pos;
	for (pos.Y = start.Y; pos.Y < end.Y; pos.Y++) {
		for (pos.X = start.X; pos.X < end.X; pos.X++) {
			SetConsoleCursorPosition(hOut, pos);
			if (pos.Y == start.Y || pos.Y == end.Y - 1 ||
				pos.X == start.X || pos.X == end.X - 1)
				printf("%c", rendmap[pos.Y][pos.X].block.tile);
			else {
				if (gamemap[pos.Y][pos.X].block.tile == CHAR_GRASS) {
					SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
					printf("%c", gamemap[pos.Y][pos.X].block.tile);
					SetConsoleTextAttribute(hOut, saved_attributes);
				}
				else if (gamemap[pos.Y][pos.X].block.tile == BL_BLOCKING) {
					SetConsoleTextAttribute(hOut, FOREGROUND_INTENSITY);
					printf("%c", gamemap[pos.Y][pos.X].block.tile);
					SetConsoleTextAttribute(hOut, saved_attributes);
				}
				else
					printf("%c", gamemap[pos.Y][pos.X].block.tile);
			}
		}
	}
	return 0;
}



static int init(System* sys, Renderer* rend, Player* plr) {
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
	rend->gamearea_start.X = rend->rendarea_start.X +2;
	rend->gamearea_start.Y = rend->rendarea_start.Y +2;
	rend->gamearea_end.X = rend->rendarea_end.X - 2;
	rend->gamearea_end.Y = rend->rendarea_end.X - 2;
	rend->statsarea_start.X = 0;
	rend->statsarea_start.Y = REN_HEADER_ROWS + REN_ROWS;
	rend->statsarea_end.X = REN_COLS;
	rend->statsarea_end.Y = REN_HEADER_ROWS + REN_ROWS + REN_STATS_ROWS;
	rend->upd_rendarea = 1;
	rend->upd_header = 1;
	rend->upd_stats = 1;

	plr->name = "Player";
	plr->health = 0;
	plr->atkbase = 0;
	plr->atkboost = 0;
	plr->defbase = 0;
	plr->defboost = 0;
	plr->dir = 9;
	plr->marker = "@";
	plr->Position.X = 10;
	plr->Position.Y = 10;
	plr->PositionOld = plr->Position;

	// Init console output handle
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	// Hide cursor
	CONSOLE_CURSOR_INFO cur;
	cur.dwSize = 100;
	cur.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &cur);

	// Generate random map
	gen_rendarea(rend);


	return 0;
}

static char input() {
	if (_kbhit())
		return _getch();
	else
		return NULL;
}




static int update(System* sys, Renderer* rend, Map* map, Player* plr) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (rend->upd_header)
		rend->upd_header = rend_upd_header(sys, rend, map, hOut);
	if (rend->upd_stats)
		rend->upd_stats = rend_upd_stats(sys, rend, plr, hOut);
	return 0;
}

static int renderer(System* sys, Renderer* rend, Map* map, Player* plr) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (rend->upd_rendarea)
		rend->upd_rendarea = draw_rendarea(rend, hOut);
	return 0;
}





static int rend_upd_stats(System* sys, Renderer* rend, Player* plr, HANDLE hOut) {
	if (sys->debug) {
		// Debug texts
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
	printf(" HP: %d\n", plr->health);
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
	int ret_init = 0, ret_update = 0, ret_renderer = 0;
	ret_init = init(psys, prend, pplr);


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
		case 'q': exit(0); break;
		}


		//update
			ret_update = update(psys, prend, pmap, pplr);


		//renderer
			ret_renderer = renderer(psys, prend, pmap, pplr);


	} // END OF MAIN LOOP
	return 0;
}
