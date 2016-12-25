/*
 * adc_sample.c
 *
 *  Created on: 24 dec. 2016
 *      Author: jancu
 */

#ifndef ELOAD_API_ADC_SAMPLE_C_
#define ELOAD_API_ADC_SAMPLE_C_

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/ADC.h>

/* Example/Board Header files */
#include "Board.h"


/* ADC conversion result variables */
// todo: make this RTOS, smart, not global vars
uint16_t adcValue0;
uint16_t adcValue1;


/*
 *  ======== fxAdcSample ========
 *  Open an ADC instance and get a sampling result from a one-shot conversion.
 */
Void fxAdcSample(UArg arg0, UArg arg1)
{
    ADC_Handle   adc0;
    ADC_Handle   adc1;
    ADC_Params   params;

    ADC_Params_init(&params);
    adc0 = ADC_open(Board_ADC0, &params);
    adc1 = ADC_open(Board_ADC1, &params);

    while (1) {
        Task_sleep(((UInt)arg0) / Clock_tickPeriod);
        /* Blocking mode conversion */
        ADC_convert(adc0, &adcValue0);
        ADC_convert(adc1, &adcValue1);
    }
    // theoretically close the ADC driver. THis code is never reached
//    ADC_close(adc0);
//    ADC_close(adc1);
}

#endif /* ELOAD_API_ADC_SAMPLE_C_ */
