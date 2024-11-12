#include "tools.h"

extern int random_number(int min, int max) {
    if (max < min) { // Switch min<->max if needed
        int temp = max;
        max = min;
        min = temp;
    }
    return rand() % (max - min + 1) + min;
}



// Function to sleep for milliseconds
extern void sleep_ms(int milliseconds) { // cross-platform sleep function
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (milliseconds >= 1000)
        sleep(milliseconds / 1000);
    usleep((milliseconds % 1000) * 1000);
#endif
}

