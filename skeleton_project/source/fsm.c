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
#include "fsm.h"
#include "driver/elevio.h"
#include "orders.h"
#include "timer.h"
#include <stddef.h>

fsm_t fsm;

static int current_floor = -1;
static MotorDirection current_direction = DIRN_STOP;

void fsm_init(void) {
    current_floor = elevio_floorSensor();
    current_direction = DIRN_STOP;
    fsm.state = state_init;
    fsm_run(EVENT_ENTRY);
}

void fsm_run(fsm_events_t event) {
    if (fsm.state == NULL) {
        return;
    }
    fsm.state(event);
}

void fsm_transition(state_fp new_state) {
    fsm_run(EVENT_EXIT);
    fsm.state = new_state;
    fsm_run(EVENT_ENTRY);
}

void state_init(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            orders_set_blocked(1);

            if (elevio_floorSensor() != -1) {
                current_floor = elevio_floorSensor();
                elevio_floorIndicator(current_floor);
                fsm_transition(state_idle);
                return;
            }

            elevio_motorDirection(DIRN_DOWN);
            break;

        case EVENT_TICK: {
            int floor = elevio_floorSensor();
            if (floor != -1) {
                current_floor = floor;
                elevio_floorIndicator(current_floor);
                fsm_transition(state_idle);
            }
            break;
        }

        case EVENT_EXIT:
            elevio_motorDirection(DIRN_STOP);
            orders_set_blocked(0);
            break;

        case EVENT_ERROR:
            break;
    }
}

void state_idle(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            elevio_motorDirection(DIRN_STOP);
            current_direction = DIRN_STOP;
            break;

        case EVENT_TICK:
            if (elevio_stopButton()) {
                fsm_transition(state_stop);
                return;
            }

            if (orders_any()) {
                if (orders_has_order_at(current_floor)) {
                    fsm_transition(state_door_open);
                    return;
                }

                current_direction = orders_choose_direction(current_floor, current_direction);
                if (current_direction != DIRN_STOP) {
                    fsm_transition(state_moving);
                }
            }
            break;

        case EVENT_EXIT:
            break;

        case EVENT_ERROR:
            fsm_transition(state_error);
            break;
    }
}

void state_moving(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            elevio_motorDirection(current_direction);
            break;

        case EVENT_TICK: {
            if (elevio_stopButton()) {
                fsm_transition(state_stop);
                return;
            }

            int floor = elevio_floorSensor();
            if (floor != -1) {
                current_floor = floor;
                elevio_floorIndicator(current_floor);

                if (orders_should_stop(current_floor, current_direction)) {
                    fsm_transition(state_door_open);
                    return;
                }
            }
            break;
        }

        case EVENT_EXIT:
            elevio_motorDirection(DIRN_STOP);
            break;

        case EVENT_ERROR:
            fsm_transition(state_error);
            break;
    }
}

void state_door_open(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            elevio_motorDirection(DIRN_STOP);
            elevio_doorOpenLamp(1);
            orders_clear_at_floor(current_floor);
            timer_start();
            break;

        case EVENT_TICK:
            if (elevio_stopButton()) {
                fsm_transition(state_stop);
                return;
            }

            if (orders_has_order_at(current_floor)) {
                orders_clear_at_floor(current_floor);
                timer_start();
                return;
            }

            if (elevio_obstruction()) {
                timer_start();
                return;
            }

            if (timer_expired()) {
                if (orders_any()) {
                    current_direction = orders_choose_direction(current_floor, current_direction);
                    if (current_direction != DIRN_STOP) {
                        fsm_transition(state_moving);
                    } else {
                        fsm_transition(state_idle);
                    }
                } else {
                    fsm_transition(state_idle);
                }
            }
            break;

        case EVENT_EXIT:
            elevio_doorOpenLamp(0);
            break;

        case EVENT_ERROR:
            fsm_transition(state_error);
            break;
    }
}

void state_stop(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            elevio_motorDirection(DIRN_STOP);
            elevio_stopLamp(1);
            orders_clear_all();
            orders_set_blocked(1);

            if (elevio_floorSensor() != -1) {
                elevio_doorOpenLamp(1);
            }
            break;

        case EVENT_TICK:
            if (!elevio_stopButton()) {
                if (elevio_floorSensor() != -1) {
                    orders_set_blocked(0);
                    fsm_transition(state_door_open);
                } else {
                    orders_set_blocked(0);
                    fsm_transition(state_idle);
                }
            }
            break;

        case EVENT_EXIT:
            elevio_stopLamp(0);
            elevio_doorOpenLamp(0);
            orders_set_blocked(0);
            break;

        case EVENT_ERROR:
            break;
    }
}

void state_error(fsm_events_t event) {
    switch (event) {
        case EVENT_ENTRY:
            elevio_motorDirection(DIRN_STOP);
            orders_clear_all();
            break;

        case EVENT_TICK:
            fsm_transition(state_init);
            break;

        case EVENT_EXIT:
            break;

        case EVENT_ERROR:
            break;
    }
}