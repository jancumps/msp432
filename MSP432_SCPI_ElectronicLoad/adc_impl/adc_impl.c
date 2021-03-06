/*
 * adc_impl.c
 *
 *  Created on: 2 mrt. 2017
 *      Author: jancu poakes
 */

/* For usleep() */
#include <unistd.h>

#include <ti/drivers/I2C.h>

/* Example/Board Header files */
#include "Board.h"

#include "i2c_impl.h"
#include "adc_impl.h"

#include "rtos_schedules.h"

#define ADC_I2C_ADDR (0x48)

// single conversion mode, +- 6.144V
#define ADS1115_CFG_H0 0b11000001
#define ADS1115_CFG_H1 0b11010001
#define ADS1115_CFG_H2 0b11100001
#define ADS1115_CFG_H3 0b11110001

#define ADS1115_MUX_DELAY 9000

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
    uint16_t raw[4]; // I only need 3 locations because the fourth channel isn't sampled. But Peter's client app also asks the ADC values of channel 4.
                     // it's easier to just provide a read of 0 than to adapt the code to deal with an attempt to read ADC 4.
                     // if you connect something to the input of ADC 4, you can adapt the code below and set ADC_ACTIVE_INPUTS to 4.
                     // it will decrease the buffer time betwen two reads from 12/3 to 1s/4.
} ADCValues;

volatile ADCValues adcRoundRobin[2];
volatile uint32_t adcRoundRobinIndex[4] = {0}; // I only need 3 locations because the fourth channel isn't sampled. But Peter's client app also asks the ADC values of channel 4.
                                               // it's easier to just provide a read of 0 than to adapt the code to deal with an attempt to read ADC 4.
                                               // if you connect something to the input of ADC 4, you can adapt the code below and set ADC_ACTIVE_INPUTS to 4.
                                               // it will decrease the buffer time betwen two reads from 12/3 to 1s/4.

// volts per step
const float VPS = 6.144 / 32768.0; // todo: is this correct for a 2v ref?

uint8_t         a_txBuffer[3];
uint8_t         a_rxBuffer[2];
I2C_Transaction a_i2cTransaction;

static const uint8_t array_ADS1115_CFG_H[4] = {ADS1115_CFG_H0, ADS1115_CFG_H1, ADS1115_CFG_H2, ADS1115_CFG_H3};



uint16_t sampleADC(uint32_t uModule);

/*
 *  ======== threadADC  ========

 */
void *threadADC(void *arg0) {
    uint32_t i;

    a_i2cTransaction.writeBuf = a_txBuffer;
    a_i2cTransaction.readBuf = a_rxBuffer;
    a_i2cTransaction.slaveAddress = ADC_I2C_ADDR;

    a_txBuffer[2] = ADS1115_CFG_L;

    while (1)
    {
        for (i =0; i< ADC_ACTIVE_INPUTS; i++) {
            // we write value to the inactive robin
            // store value of ADC[i]
            adcRoundRobin[adcRoundRobinIndex[i] ? 0 : 1].raw[i] = sampleADC(i);
            // after value(s) written, we activate the inactive robin
            adcRoundRobinIndex[i] = adcRoundRobinIndex[i] ? 0 : 1;
        }

        // commented because mcu spends more time in these lines than in the remainder
        // enable when needed, then comment out again
        // check the schedule before uncommenting. Should be 1s or more
        //            System_printf("ADCValue= %d, ADCVolts= %f \n",
        //                          adcImplToValue(adcRoundRobin[adcRoundRobinIndex ? 0 : 1].raw[0]),
        //                          adcImplToFloat(adcRoundRobin[adcRoundRobinIndex ? 0 : 1].raw[0]) );
        //            System_flush();


        // the thread delays ADS1115_MUX_DELAY per sample
        // so let's deduct the total delay from the requested sleep time
        usleep(THREAD_USLEEP_ADC -  (unsigned int)(ADS1115_MUX_DELAY * ADC_ACTIVE_INPUTS));
    }
}

uint16_t adcImplGetAdc(uint32_t uModule) {
    return adcRoundRobin[adcRoundRobinIndex[uModule]].raw[uModule];
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

uint16_t sampleADC(uint32_t uModule) {
    uint16_t uRetval = 0u;

    /* Init ADC and Start Sampling */
    a_txBuffer[1] = array_ADS1115_CFG_H[uModule];

    a_txBuffer[0] = 0x01;
    a_i2cTransaction.writeCount = 3;
    a_i2cTransaction.readCount = 0;
    if (! I2C_transfer(i2c_implGetHandle(), &a_i2cTransaction)){
      //        System_printf("Sampling Start Failed \n");
    }

    // inspiration: https://github.com/adafruit/Adafruit_ADS1X15/blob/master/Adafruit_ADS1015.cpp
    usleep(ADS1115_MUX_DELAY);

    /* Read ADC */
    a_txBuffer[0] = 0x00;
    a_i2cTransaction.writeCount = 1;
    a_i2cTransaction.readCount = 2;
    if (I2C_transfer(i2c_implGetHandle(), &a_i2cTransaction)) {
        uRetval = ((a_rxBuffer[0] << 8) | a_rxBuffer[1]);
    }     else {
//        System_printf("ADC Read I2C Bus fault\n");
    }

    return uRetval;
}

