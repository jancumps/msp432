/*
 * pid_impl.c
 *
 *  Created on: 13 jan. 2017
 *      Author: jancu
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

/* TI-RTOS Header files */


#include "pid.h"

/*
 *  ======== fnTaskPIDy ========
 *
 */
Void fnTaskPID(UArg arg0, UArg arg1)
{
    //Define Variables we'll be connecting to
    double Setpoint, Input, Output;

    //Specify the links and initial tuning parameters
    double Kp=2, Ki=5, Kd=1;
    pidInit(&Input, &Output, &Setpoint, Kp, Ki, Kd, PID_DIRECT);

    //initialize the variables we're linked to
//     Input = analogRead(PIN_INPUT); todo: get a value
     Setpoint = 100;

     //turn the PID on
     pidSetMode(PID_AUTOMATIC);


    while (1) {
        Task_sleep(((UInt)arg0) / Clock_tickPeriod);


//        Input = analogRead(PIN_INPUT); todo: get a value
        pidCompute();
//        analogWrite(PIN_OUTPUT, Output);   todo: can go?


    }
}



