#include "main.h"
#include "tools.h"
#include <conio.h>
#include <stdio.h>
#include <time.h>


static void gen_gamearea() {
	int num_blocks = sizeof(blocks) / sizeof(blocks[0]);
	for (int r = 0; r < G_AREA_ROWS; r++)
		for (int c = 0; c < G_AREA_COLS; c++)
			gamemap[r][c].block = blocks[random_number(0, num_blocks)];
}

static void gen_rendarea_borders(Renderer* rend) {
	int start_x = rend->rendarea_start.X;
	int start_y = rend->rendarea_start.Y;
	int end_x = rend->rendarea_end.X;
	int end_y = rend->rendarea_end.Y;
	for (int r = start_y; r < end_y; r++)
		for (int c = start_x; c < end_x; c++)
			if (r == start_y)            // == The first row ==
				if (c == start_x)        // Upper left corner
					rendmap[r][c].block = walls[0];
				else if (c == end_x -1) // Upper right corner
					rendmap[r][c].block = walls[2];
				else                     // Upper center characters
					rendmap[r][c].block = walls[1];
			else if (r == end_y - 1)     // == The last row ==
				if (c == start_x)        // Lower left corner
					rendmap[r][c].block = walls[3];
				else if (c == end_x -1) // Lower right corner
					rendmap[r][c].block = walls[5];
				else                     // Lower center characters
					rendmap[r][c].block = walls[4];
}

static void draw_rendarea_borders(Renderer* rend, HANDLE* hOut) {
	SetConsoleCursorPosition(hOut, rend->rendarea_start);
	int start_x = rend->rendarea_start.X;
	int start_y = rend->rendarea_start.Y;
	int end_x = rend->rendarea_end.X;
	int end_y = rend->rendarea_end.Y;
	for (int r = start_y; r < end_y; r++) {
		for (int c = start_x; c < end_x; c++) {
			printf("%c", rendmap[r][c].block.tile);
		}
		printf("\n");
	}
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
	rend->gamearea_start.X = rend->rendarea_start.X +1;
	rend->gamearea_start.Y = rend->rendarea_start.Y +1;
	rend->gamearea_end.X = rend->rendarea_end.X - 1;
	rend->gamearea_end.Y = rend->rendarea_end.X - 1;
	rend->statsarea_start.X = 0;
	rend->statsarea_start.Y = REN_HEADER_ROWS + REN_ROWS;
	rend->statsarea_end.X = REN_COLS;
	rend->statsarea_end.Y = REN_HEADER_ROWS + REN_ROWS + REN_STATS_ROWS;
	rend->upd_rendarea = 1;
	rend->upd_header = 1;
	rend->upd_gamearea = 1;
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
	gen_gamearea();
	// Generate game area borders
	gen_rendarea_borders(rend);

	return 0;
}

static char input() {
	if (_kbhit())
		return _getch();
	else
		return NULL;
}




static int update() {

	return 0;
}



static int rend_upd_header(System* sys, Renderer* rend, HANDLE hOut) {
	if (sys->debug) {
		// Debug texts
		COORD cd = rend->header_start;
		cd.X += 49;
		SetConsoleCursorPosition(hOut, cd);
		printf("H-DEBUG: 12345678901234567890");
	}
	SetConsoleCursorPosition(hOut, rend->header_start);
	printf(" ---=== HEADER ROW ===--- ");
	return 0;
}

static int rend_upd_gamearea(Renderer* rend, HANDLE hOut) {
	int start_x = rend->rendarea_start.X;
	int start_y = rend->rendarea_start.Y;
	int end_x = rend->rendarea_end.X;
	int end_y = rend->rendarea_end.Y;

	// Print visible part of map
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;
	/* Save current attributes */
	GetConsoleScreenBufferInfo(hOut, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	SetConsoleCursorPosition(hOut, rend->rendarea_start);
	for (int r = start_y; r < end_y; r++) {
		for (int c = start_x; c < end_x; c++) {
			if (gamemap[r][c].block.tile == CHAR_GRASS) {
				SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
				printf("%c", gamemap[r][c].block.tile);
				SetConsoleTextAttribute(hOut, saved_attributes);
			}
			else if (gamemap[r][c].block.tile == BL_BLOCKING) {
				SetConsoleTextAttribute(hOut, FOREGROUND_INTENSITY);
				printf("%c", gamemap[r][c].block.tile);
				SetConsoleTextAttribute(hOut, saved_attributes);
			}
			else
				printf("%c", gamemap[r][c].block.tile);
		}
		printf("\n");
	}
	return 0;
}

static int upd_rend_gamearea_borders(Renderer* rend, HANDLE hOut) {
	gen_rendarea_borders(rend);
	draw_rendarea_borders(rend, hOut);
	// Print the borders?
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
	int atk = plr->atkbase + plr->atkboost;
	int def = plr->defbase + plr->defboost;
	printf(" HP: %d\n", plr->health);
	printf("Atk: %d   Def: %d", atk, def);
	return 0;
}

static int renderer(System* sys, Renderer* rend, Player* plr) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (rend->upd_header)
		rend->upd_header = rend_upd_header(sys, rend, hOut);
	if (rend->upd_rendarea)
		rend->upd_rendarea = rend_upd_gamearea(rend, hOut);
	if (rend->upd_gamearea)
		rend->upd_gamearea = upd_rend_gamearea_borders(rend, hOut);
	if (rend->upd_stats)
		rend->upd_stats = rend_upd_stats(sys, rend, plr, hOut);
	return 0;
}



int main(int argc, char* argv[]) {

	//init
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

		// printf("Cycles: %d\n", cycles);


		//input
		switch (input()) {
		case 'q': exit(0); break;
		//default: continue;
		}


		//update
			ret_update = update();


		//renderer
			ret_renderer = renderer(psys, prend, pplr);


	} // END OF MAIN LOOP
	return 0;
}
