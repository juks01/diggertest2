#include "main.h"
#include "tools.h"
#include <conio.h>
#include <stdio.h>
#include <time.h>


static void gen_rendarea(Renderer* rend) {
	COORD start = rend->rendarea_start;
	COORD end = rend->rendarea_end;
	COORD pos;
	int num_blocks = sizeof(blocks) / sizeof(blocks[0]);
	unsigned char header[] =
		"---=== HEADER ROW ===---                         H-DEBUG: 12345678901234567890";

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
				else // Game area between borders
					gamemap[pos.Y][pos.X].block = blocks[random_number(0, num_blocks)];
			}
	}
}



static int draw_rendarea(Renderer* rend, HANDLE* hOut) {
	COORD start = rend->rendarea_start;
	COORD end = rend->rendarea_end;

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;
	// Save current attributes
	GetConsoleScreenBufferInfo(hOut, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;

	COORD pos;
	for (pos.Y = 0; pos.Y < end.Y; pos.Y++) {
		for (pos.X = 0; pos.X < end.X; pos.X++) {
			SetConsoleCursorPosition(hOut, pos);

			// Draw upper and lower row chars of gamearea "box" and
			// left and right column chars of gamearea "box"
			if (pos.Y == start.Y || pos.Y == end.Y ||
				pos.X == start.X || pos.X == pos.X)
				printf("%c", rendmap[pos.Y][pos.X].block.tile);

			// Draw grass characters in green
			if (gamemap[pos.Y][pos.X].block.tile == CHAR_GRASS) {
				SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
				printf("%c", gamemap[pos.Y][pos.X].block.tile);
			}
			// Draw blocking characters in white
			else if (gamemap[pos.Y][pos.X].block.type == BL_BLOCKING) {
				SetConsoleTextAttribute(hOut, FOREGROUND_INTENSITY);
				printf("%c", gamemap[pos.Y][pos.X].block.tile);
			}
			// Draw other characters in grey
			else
				printf("%c", gamemap[pos.Y][pos.X].block.tile);
			SetConsoleTextAttribute(hOut, saved_attributes);
		}
	}
	return 0;
}



static int init(System* sys, Renderer* rend, Player* plr, HANDLE* hOut) {
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
	rend->gamearea_start.X = rend->rendarea_start.X + 2;
	rend->gamearea_start.Y = rend->rendarea_start.Y + 2;
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
	plr->marker = "@";
	plr->dir = 9;
	plr->Position.X = 10;
	plr->Position.Y = 10;
	plr->PositionOld = plr->Position;
	plr->level = 1;
	plr->exp = 0;
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
	gen_rendarea(rend);

	return 0;
}

static char input() {
	if (_kbhit())
		return _getch();
	else
		return NULL;
}




static int update(System* sys, Renderer* rend, Map* map, Player* plr, HANDLE hOut) {
	if (rend->upd_stats)
		rend->upd_stats = rend_upd_stats(sys, rend, plr, hOut);
	return 0;
}

static int renderer(System* sys, Renderer* rend, Map* map, Player* plr, HANDLE hOut) {
	if (rend->upd_rendarea)
		rend->upd_rendarea = draw_rendarea(rend, hOut);
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
	printf(" HP: %d             Lvl: %d   Exp: %d\n", plr->health, plr->level, plr->exp);
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
		case 'q': exit(0); break;
		}


		//update
		ret_update = update(psys, prend, pmap, pplr, hOut);


		//renderer
		ret_renderer = renderer(psys, prend, pmap, pplr, hOut);


	} // END OF MAIN LOOP
	return 0;
}
