/*
+--------------------------------- +
|  >>Date                          |
|    27.02.26                      |.
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
#ifndef FSM_H
#define FSM_H

typedef enum {
    EVENT_TICK,
    EVENT_ENTRY,
    EVENT_EXIT,
    EVENT_ERROR
} fsm_events_t;

typedef void (*state_fp)(fsm_events_t event);

typedef struct {
    state_fp state;
} fsm_t;

extern fsm_t fsm;

void fsm_init(void);
void fsm_run(fsm_events_t event);
void fsm_transition(state_fp new_state);

void state_init(fsm_events_t event);
void state_idle(fsm_events_t event);
void state_moving(fsm_events_t event);
void state_door_open(fsm_events_t event);
void state_stop(fsm_events_t event);
void state_error(fsm_events_t event);

#endif