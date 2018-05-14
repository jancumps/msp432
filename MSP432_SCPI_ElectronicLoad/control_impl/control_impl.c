/*
 * control_impl.c
 *
 *  Created on: 15 aug. 2017
 *      Author: jancu
 */



#include <unistd.h>
#include "eload_api.h"

#include "rtos_schedules.h"

/*
 *  ======== fnTaskControl ========
 *  Run control loop.
 *  Frequency is dependent on the control strategy.
 *  If the strategy doesn't need a loop, run at the time defined by THREAD_USLEEP_CONTROL
 *  todo: maybe exit and start the task from the strategy, and don't run when not needed?
 */
void *threadControl(void *arg0)
{

    uint32_t uDefaultWait = THREAD_USLEEP_CONTROL;


    while (1) {
        uint32_t uWait = eloadControlLoopGetSchedule();
        eloadControlLoop();

        usleep(uWait ?  uWait : uDefaultWait);

    }
}
