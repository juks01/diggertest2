#include "main.h"
#include "tools.h"
#include <conio.h>
#include <stdio.h>
#include <time.h>


static int init(System* sys, Renderer* rend, Map* m, Player* plr) {
	sys->debug = SYS_DEBUG;
	sys->show_fps = 1;
	sys->show_frames = 1;

	rend->header_start.X = 0;
	rend->header_start.Y = 0;
	rend->header_end.X = REN_COLS;
	rend->header_end.Y = REN_HEADER_ROWS;
	rend->gamearea_start.X = 0;
	rend->gamearea_start.Y = REN_HEADER_ROWS;
	rend->gamearea_end.X = REN_COLS;
	rend->gamearea_end.Y = REN_HEADER_ROWS + REN_ROWS;
	rend->statsarea_start.X = 0;
	rend->statsarea_start.Y = REN_HEADER_ROWS + REN_ROWS;
	rend->statsarea_end.X = REN_COLS;
	rend->statsarea_end.Y = REN_HEADER_ROWS + REN_ROWS + REN_STATS_ROWS;
	rend->upd_header = 1;
	rend->upd_gamearea = 1;
	rend->upd_stats = 1;

	plr->name = "Player";
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



static int renderer() {

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
	
	int ret_init = init(psys, prend, pmap, pplr);

	// THE MAIN LOOP
	int cycles = 0;
	while (1) {
		sleep_ms(SYS_CYCLES);
		if (cycles < 5000)
			cycles++;
		else
			cycles = 0;

		printf("Cycles: %d\n", cycles);


		//input
		switch (input()) {
		case 'q': break;
		default: continue;
		}


		//update
			int ret_update = update();


		//renderer
			int ret_renderer = renderer();


	} // END OF MAIN LOOP
	return 0;
}
