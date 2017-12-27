/*
 * adc_impl.c
 *
 *  Created on: 2 mrt. 2017
 *      Author: jancu poakes
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
//#include <xdc/cfg/global.h> // needed to get the global from the .cfg file

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
//#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Example/Board Header files */
#include "Board.h"

#include "i2c_impl.h"
#include "adc_impl.h"

#define ADC_I2C_ADDR (0x48)

// bits 15-8, 0xC2 Continuous conversion mode, AIN0, +- 6.144V
#define ADS1115_CFG_H0 0b11000000
#define ADS1115_CFG_H1 0b11010000
#define ADS1115_CFG_H2 0b11100000
#define ADS1115_CFG_H3 0b11110000

// adc 4 is not used. Sample 3 channels
#define ADC_ACTIVE_INPUTS 3




// bits 7-0  0x85 128SPS, Disable comparator
// todo: check this, because this is 0x83. What's the best setting for ADC?
#define ADS1115_CFG_L 0b10000011

// we capture the ADC values in a round robin array of two positions
// read will always happen from the one that's not written to
// the non-read buffer is available for writing, and is marked as the active read buffr after that
// adcRoundRobinIndex points to the active read buffer
// if you use getAdc() to retrieve values, this is sorted out for you

typedef struct ADCValues {
    uint16_t raw[4];
} ADCValues;

volatile ADCValues adcRoundRobin[2];
volatile uint32_t adcRoundRobinIndex = 0;

// volts per step
const float VPS = 6.144 / 32768.0; // todo: is this correct for a 2v ref?

uint8_t         a_txBuffer[3];
uint8_t         a_rxBuffer[2];
I2C_Transaction a_i2cTransaction;

static const uint8_t array_ADS1115_CFG_H[4] = {ADS1115_CFG_H0, ADS1115_CFG_H1, ADS1115_CFG_H2, ADS1115_CFG_H3};



uint16_t sampleADC(uint32_t uModule, UInt uSleep);

/*
 *  ======== fnTaskADC ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Void fnTaskADC(UArg arg0, UArg arg1)
{
    uint32_t i;

    a_i2cTransaction.writeBuf = a_txBuffer;
    a_i2cTransaction.readBuf = a_rxBuffer;
    a_i2cTransaction.slaveAddress = ADC_I2C_ADDR;

    // this buffer value never changes. Let's set it at the start.
    // If for some reason this becomes a variable value,
    // move to sampleADC()
    a_txBuffer[2] = ADS1115_CFG_L;

    while (1)
    {
        for (i =0; i< ADC_ACTIVE_INPUTS; i++) {
            // we write value to the inactive robin
            // store value of ADC[i]
            // the ADC needs time between channel selection and sampling
            // we assign 1/4 of the task sleep time to
            // each of the 4 samples
            // this puts more burden on the RTOS switcher - a compromise
            // - but certainly preferable to a loop
            // (except when later on we find out that the wait is only a few cpu cycles)
            adcRoundRobin[adcRoundRobinIndex ? 0 : 1].raw[i] = sampleADC(i, (UInt)arg0/ADC_ACTIVE_INPUTS);
        }


        // after value(s) written, we activate the inactive robin
        adcRoundRobinIndex = adcRoundRobinIndex ? 0 : 1;

        // commented because mcu spends more time in these lines than in the remainder
        // enable when needed, then comment out again
        // check the schedule before uncommenting. Should be 1s or more
        //            System_printf("ADCValue= %d, ADCVolts= %f \n",
        //                          adcImplToValue(adcRoundRobin[adcRoundRobinIndex ? 0 : 1].raw[0]),
        //                          adcImplToFloat(adcRoundRobin[adcRoundRobinIndex ? 0 : 1].raw[0]) );
        //            System_flush();


    }

}

uint16_t adcImplGetAdc(uint32_t uModule) {
    return adcRoundRobin[adcRoundRobinIndex].raw[uModule];
}

uint16_t adcImplToValue(uint16_t uRaw){
    uint16_t        ADCValue;
    bool            ValNegative = false;
    ADCValue = uRaw;
    ValNegative = (0x8000&ADCValue ? true : false);
    ADCValue = (ValNegative ? 0x8000 - (int)(0x7FFF&ADCValue) : ADCValue) ;
    return ADCValue;
}

float adcImplToFloat(uint16_t uRaw) {
    uint16_t        ADCValue;
    float           ADCVoltage;
    bool            ValNegative = false;
    ADCValue = uRaw;
    ValNegative = (0x8000&ADCValue ? true : false);
    ADCValue = (ValNegative ? 0x8000 - (int)(0x7FFF&ADCValue) : ADCValue) ;
    ADCVoltage = (ValNegative ? (ADCValue * VPS) *-1: ADCValue * VPS ) ;
    return ADCVoltage;
}


uint16_t sampleADC(uint32_t uModule, UInt uSleep) {
    uint16_t uRetval = 0u;

    /* Point to the ADC ASD1115 and read input uModule */
    a_i2cTransaction.writeCount = 3;
    a_i2cTransaction.readCount = 0;
    a_txBuffer[0] = 0x01;
    a_txBuffer[1] = array_ADS1115_CFG_H[uModule];



    /* Init ADC and Start Sampling */
    if (! I2C_transfer(i2c_implGetHandle(), &a_i2cTransaction)){
        System_printf("Sampling Start Failed \n");
    }

    // there's a pause required between channel selection and data retrieval
    // we consume that part of the task sleep time that's assigned to us by the task.
    Task_sleep(uSleep);

    a_txBuffer[0] = 0x00;
    a_i2cTransaction.writeCount = 1;
    a_i2cTransaction.readCount = 2;
    /* Read ADC */
    if (I2C_transfer(i2c_implGetHandle(), &a_i2cTransaction)) {
        uRetval = ((a_rxBuffer[0] << 8) | a_rxBuffer[1]);
    }
    else {
        System_printf("ADC Read I2C Bus fault\n");
    }



    return uRetval;
}

