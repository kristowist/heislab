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
#include "orders.h"
#include <stdbool.h>


static int orders[N_FLOORS][N_BUTTONS] = {0};
static int orders_blocked = 0;

void orders_init(void) {
    orders_clear_all();
    orders_blocked = 0;
}

void orders_set_blocked(int blocked) {
    orders_blocked = blocked;
}

void orders_add(int floor, ButtonType button) {
    if (orders_blocked) {
        return;
    }

    if (floor < 0 || floor >= N_FLOORS) {
        return;
    }

    if (floor == N_FLOORS - 1 && button == BUTTON_HALL_UP) {
        return;
    }
    if (floor == 0 && button == BUTTON_HALL_DOWN) {
        return;
    }

    orders[floor][button] = 1;
    elevio_buttonLamp(floor, button, 1);
}

void orders_clear_at_floor(int floor) {
    if (floor < 0 || floor >= N_FLOORS) {
        return;
    }

    for (int b = 0; b < N_BUTTONS; b++) {
        orders[floor][b] = 0;
        elevio_buttonLamp(floor, b, 0);
    }
}

void orders_clear_all(void) {
    for (int f = 0; f < N_FLOORS; f++) {
        for (int b = 0; b < N_BUTTONS; b++) {
            orders[f][b] = 0;
            elevio_buttonLamp(f, b, 0);
        }
    }
}

bool orders_has_order_at(int floor) {
    if (floor < 0 || floor >= N_FLOORS) {
        return false;
    }

    for (int b = 0; b < N_BUTTONS; b++) {
        if (orders[floor][b]) {
            return true;
        }
    }
    return false;
}

bool orders_any(void) {
    for (int f = 0; f < N_FLOORS; f++) {
        if (orders_has_order_at(f)) {
            return true;
        }
    }
    return false;
}

bool orders_above(int floor) {
    for (int f = floor + 1; f < N_FLOORS; f++) {
        if (orders_has_order_at(f)) {
            return true;
        }
    }
    return false;
}

bool orders_below(int floor) {
    for (int f = 0; f < floor; f++) {
            if (orders_has_order_at(f)) {
                return true;
        }
    }
    return false;
}

bool orders_should_stop(int floor, MotorDirection direction) {
    if (floor < 0 || floor >= N_FLOORS) {
        return false;
    }

    if (orders[floor][BUTTON_CAB]) {
        return true;
    }

    if (direction == DIRN_UP && orders[floor][BUTTON_HALL_UP]) {
        return true;
    }
    if (direction == DIRN_DOWN && orders[floor][BUTTON_HALL_DOWN]) {
        return true;
    }

    if (direction == DIRN_UP && !orders_above(floor)) {
        if (orders[floor][BUTTON_HALL_DOWN]) {
            return true;
        }
    }
    if (direction == DIRN_DOWN && !orders_below(floor)) {
        if (orders[floor][BUTTON_HALL_UP]) {
            return true;
        }
    }

    if (floor == 0 || floor == N_FLOORS - 1) {
        if (orders_has_order_at(floor)) {
            return true;
        }
    }

    return false;
}

MotorDirection orders_choose_direction(int floor, MotorDirection current_direction) {
    if (current_direction == DIRN_UP) {
        if (orders_above(floor)) {
            return DIRN_UP;
        }
        if (orders_below(floor)) {
            return DIRN_DOWN;
        }
    }

    if (current_direction == DIRN_DOWN) {
        if (orders_below(floor)) {
            return DIRN_DOWN;
        }
        if (orders_above(floor)) {
            return DIRN_UP;
        }
    }

    if (current_direction == DIRN_STOP) {
        if (orders_above(floor)) {
            return DIRN_UP;
        }
        if (orders_below(floor)) {
            return DIRN_DOWN;
        }
    }

    return DIRN_STOP;
}

void orders_poll(void) {
    for (int f = 0; f < N_FLOORS; f++) {
        for (int b = 0; b < N_BUTTONS; b++) {
            if (elevio_callButton(f, b)) {
                orders_add(f, b);
            }
        }
    }
}
