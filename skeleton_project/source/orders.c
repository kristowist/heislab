#include "orders.h"


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

int orders_has_order_at(int floor) {
    if (floor < 0 || floor >= N_FLOORS) {
        return 0;
    }

    for (int b = 0; b < N_BUTTONS; b++) {
        if (orders[floor][b]) {
            return 1;
        }
    }
    return 0;
}

int orders_any(void) {
    for (int f = 0; f < N_FLOORS; f++) {
        if (orders_has_order_at(f)) {
            return 1;
        }
    }
    return 0;
}

int orders_any_above(int floor) {
    for (int f = floor + 1; f < N_FLOORS; f++) {
        if (orders_has_order_at(f)) {
            return 1;
        }
    }
    return 0;
}

int orders_any_below(int floor) {
    for (int f = 0; f < floor; f++) {
        if (orders_has_order_at(f)) {
            return 1;
        }
    }
    return 0;
}

int orders_should_stop(int floor, MotorDirection direction) {
    if (floor < 0 || floor >= N_FLOORS) {
        return 0;
    }

    if (orders[floor][BUTTON_CAB]) {
        return 1;
    }

    if (direction == DIRN_UP && orders[floor][BUTTON_HALL_UP]) {
        return 1;
    }
    if (direction == DIRN_DOWN && orders[floor][BUTTON_HALL_DOWN]) {
        return 1;
    }

    if (direction == DIRN_UP && !orders_any_above(floor)) {
        if (orders[floor][BUTTON_HALL_DOWN]) {
            return 1;
        }
    }
    if (direction == DIRN_DOWN && !orders_any_below(floor)) {
        if (orders[floor][BUTTON_HALL_UP]) {
            return 1;
        }
    }

    if (floor == 0 || floor == N_FLOORS - 1) {
        if (orders_has_order_at(floor)) {
            return 1;
        }
    }

    return 0;
}

MotorDirection orders_choose_direction(int floor, MotorDirection current_direction) {
    if (current_direction == DIRN_UP) {
        if (orders_any_above(floor)) {
            return DIRN_UP;
        }
        if (orders_any_below(floor)) {
            return DIRN_DOWN;
        }
    }

    if (current_direction == DIRN_DOWN) {
        if (orders_any_below(floor)) {
            return DIRN_DOWN;
        }
        if (orders_any_above(floor)) {
            return DIRN_UP;
        }
    }

    if (current_direction == DIRN_STOP) {
        if (orders_any_above(floor)) {
            return DIRN_UP;
        }
        if (orders_any_below(floor)) {
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
