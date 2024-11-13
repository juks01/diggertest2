#pragma once

#include <time.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#include <ncurses.h> // for cursot positioning etc
#endif

