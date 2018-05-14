/*
 * temperature_impl.c
 *
 *  Created on: 23 dec. 2017
 *      Author: jancu
 */



#include <stdbool.h>
#include <unistd.h>

#include "eload_api.h"
#include "temperature_impl.h"
#include "calibration_impl.h"

#include "rtos_schedules.h"

/*
 *  ======== threadTemperatureOverProtection ========
 */
void *threadTemperatureOverProtection(void *arg0) {
    float fSample;
    while (1)
    {
        if (calibrationGetTemperatureMaxResistance()) { // if enabled
            fSample = eloadCalibrateGetAdcVolt(2);
            if (fSample > 0.0) { // ignore if no samples been taken yet
                if ( fSample < calibrationGetTemperatureMaxResistanceFloat()) { // todo this should be a uint calculation
                    eloadInputEnable(false);
                }
            }
        }
        sleep(THREAD_SLEEP_TEMPERATUREOVERPROTECTION);
    }
}
