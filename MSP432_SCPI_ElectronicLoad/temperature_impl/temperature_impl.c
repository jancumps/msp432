/*
 * temperature_impl.c
 *
 *  Created on: 23 dec. 2017
 *      Author: jancu
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
//#include <xdc/cfg/global.h> // needed to get the global from the .cfg file

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <stdbool.h>

#include "eload_api.h"
#include "temperature_impl.h"
#include "calibration_impl.h"

/*
 *  ======== fnTaskTemperatureOverProtection ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Void fnTaskTemperatureOverProtection(UArg arg0, UArg arg1)
{
    float fSample;
    while (1)
    {
        if (calibrationGetTemperatureMaxResistance()) { // if enabled
            fSample = eLoadDevelopGetAdcVolt(2);
            if (fSample > 0.0) { // ignore if no samples been taken yet
                if ( fSample < calibrationGetTemperatureMaxFloat()) { // todo this should be a uint calculation
                    eloadInputEnable(false);
                }
            }
        }
        Task_sleep(arg0);
    }
}
