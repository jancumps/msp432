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
#include "eload_api.h"

//Define Variables we'll be connecting to
double Setpoint, Input, Output;


void strategyConstantCurrentInit(UInt sampleTime, double setPoint);
void strategyConstantCurrent();

/*
 *  ======== fnTaskPID ========
 *
 */
Void fnTaskPID(UArg arg0, UArg arg1)
{

    switch (eloadGetMode()) {
    case ELOAD_MODE_CURRENT:
        strategyConstantCurrentInit(((UInt)arg0) / Clock_tickPeriod, 0.0);
        break;
    default:
        System_exit(-1); // todo:implement other modes
        break;
     }

//     Input = analogRead(PIN_INPUT); todo: get a value


     //turn the PID on
     pidSetMode(PID_AUTOMATIC);


    while (1) {
        Task_sleep(((UInt)arg0) / Clock_tickPeriod);

        switch (eloadGetMode()) {
        case ELOAD_MODE_CURRENT:
            strategyConstantCurrent();
            break;
        default:
            System_exit(-1); // todo:implement other modes
            break;
         }

    }
}


void strategyConstantCurrentInit(UInt sampleTime, double setPoint) {
    //Specify the links and initial tuning parameters
    double Kp=2, Ki=5, Kd=1; // todo make these good values for our strategy
    Setpoint = setPoint;
    pidSetSampleTime(sampleTime);
    pidSetOutputLimits(0, eLoadGetOutputRangeMax());
    pidInit(&Input, &Output, &Setpoint, Kp, Ki, Kd, PID_DIRECT);



}

void strategyConstantCurrent() {
    //        Input = analogRead(PIN_INPUT); todo: get a value
            if (pidCompute()) {
    //        analogWrite(PIN_OUTPUT, Output);   todo: this must be replaced by the change to the eload
            }

}


