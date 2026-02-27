#pragma once

#include "driver/elevio.h"

void orders_init(void);
void orders_set_blocked(int blocked);

void orders_add(int floor, ButtonType button);
void orders_clear_at_floor(int floor);
void orders_clear_all(void);

int orders_has_order_at(int floor);
int orders_any(void);
int orders_any_above(int floor);
int orders_any_below(int floor);

int orders_should_stop(int floor, MotorDirection direction);
MotorDirection orders_choose_direction(int floor, MotorDirection current_direction);

void orders_poll(void);
