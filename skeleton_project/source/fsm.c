/*
+--------------------------------- +
|  >>Date                          |
|    04.02.26                      |.
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
#include "fsm.h"

fsm_t fsm;

void fsm_run(fsm_events_t event) {
    fsm.state(event);
}

void fsm_transition(state_fp new_state) {
    fsm_run(EVENT_EXIT);
    fsm.state = new_state;
    fsm_run(EVENT_ENTRY);
}