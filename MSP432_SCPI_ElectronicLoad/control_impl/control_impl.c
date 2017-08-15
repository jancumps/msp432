/*
 * control_impl.c
 *
 *  Created on: 15 aug. 2017
 *      Author: jancu
 */


/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>



/*
 *  ======== fnTaskControl ========
 *  Run control loop.
 *  Frequency is dependent on the control strategy.
 *  If the strategy doesn't need a loop, run at the time defined by parameter 1
 *  todo: maybe exit and start the task from the strategy, and don't run when not needed?
 */
Void fnTaskControl(UArg arg0, UArg arg1)
{

    UInt uDefaultWait = (UInt)arg0;


    while (1) {
        // todo: ask strategy how often it should refresh
        // todo: call strategy's control implementation
        Task_sleep(uDefaultWait);
    }
}
