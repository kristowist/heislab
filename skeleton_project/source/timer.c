
/*
+--------------------------------- +
|  >>Date                          |
|    01.03.26                      |.
|  >>git pulled by                 |.
|    Gabriel Sebastian Steeneveldt |.
+--------------------------------- _____
                                  |aw tysm|
               ___    ___         |_____ |
              (@  )__(  @)       /  /
             /   _______  \     /  /
            (   (______)   )   /  /
            \_____________/   /  /
*/
#include "timer.h"
#include <time.h>

static time_t door_open_time = 0;
static int timer_active = 0;

void timer_start(void) {
    door_open_time = time(NULL);
    timer_active = 1;
}

int timer_expired(void) {
    if (!timer_active) {
        return 0;
    }
    return (time(NULL) - door_open_time) >= DOOR_OPEN_DURATION;
}
