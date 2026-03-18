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
#pragma once

#include "driver/elevio.h"
#include <stdbool.h>

void orders_init(void);
void orders_set_blocked(int blocked);

void orders_add(int floor, ButtonType button);
void orders_clear_at_floor(int floor);
void orders_clear_all(void);

bool orders_has_order_at(int floor);
bool orders_any(void);
bool orders_above(int floor);
bool orders_below(int floor);

bool orders_should_stop(int floor, MotorDirection direction);
MotorDirection orders_choose_direction(int floor, MotorDirection current_direction);

void orders_poll(void);
