#include <stdio.h>
#include <time.h>

#include "driver/elevio.h"
#include "fsm.h"
#include "orders.h"

int main(void) {
    elevio_init();
    orders_init();
    fsm_init();

    while (1) {
        orders_poll();
        fsm_run(EVENT_TICK);

        nanosleep(&(struct timespec){0, 10 * 1000 * 1000}, NULL);
    }

    return 0;
}
