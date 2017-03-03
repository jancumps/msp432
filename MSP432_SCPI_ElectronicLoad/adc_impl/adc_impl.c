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
#define ADS1115_CFG_H 0b11000000
// bits 7-0  0x85 128SPS, Disable comparator
#define ADS1115_CFG_L 0b10000011
// volts per step
const float VPS = 6.144 / 32768.0; // todo: is this correct for a 2v ref?


// we capture the ADC values in a round robin array of two positions
// read will always happen from the one that's not written to
// the non-read buffer is available for writing, and is marked as the active read buffr after that
// adcRoundRobinIndex points to the active read buffer
// if you use getAdc() to retrieve values, this is sorted out for you

typedef struct ADCValues {
    uint16_t value[4];
} ADCValues;

volatile ADCValues adcRoundRobin[2];
volatile uint32_t adcRoundRobinIndex = 0;

uint8_t         a_txBuffer[3];
uint8_t         a_rxBuffer[2];
I2C_Transaction a_i2cTransaction;

/*
 *  ======== fnTaskADC ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Void fnTaskADC(UArg arg0, UArg arg1)
{
    uint16_t        ADCValue;
    float           ADCVoltage;
    bool            ValNegative = false;

    a_i2cTransaction.writeBuf = a_txBuffer;
    a_i2cTransaction.readBuf = a_rxBuffer;
    a_i2cTransaction.slaveAddress = ADC_I2C_ADDR;



    /* Point to the ADC ASD1115 and read input 0 */
    // ANC1 and GND, 4.096v, 128s/s
    a_i2cTransaction.writeCount = 3;
    a_i2cTransaction.readCount = 0;
    a_txBuffer[0] = 0x01;
    a_txBuffer[1] = ADS1115_CFG_H;
    a_txBuffer[2] = ADS1115_CFG_L;



    /* Init ADC and Start Sampling */
    if (! I2C_transfer(i2c_implGetHandle(), &a_i2cTransaction)){
        System_printf("Sampling Start Failed \n");
    }
    Task_sleep(500); // todo : do we need to pause here?
    while (1)
    {
        Task_sleep((UInt)arg0);
        a_txBuffer[0] = 0x00;
        a_i2cTransaction.writeCount = 1;
        a_i2cTransaction.readCount = 2;
        /* Read ADC */
        if (I2C_transfer(i2c_implGetHandle(), &a_i2cTransaction)) {
            /* Extract degrees C from the received data; see TMP102 datasheet */
            ADCValue = (a_rxBuffer[0] << 8) | a_rxBuffer[1];
            ValNegative = (0x8000&ADCValue ? true : false);
            ADCValue = (ValNegative ? 0x8000 - (int)(0x7FFF&ADCValue) : ADCValue) ;
            ADCVoltage = (ValNegative ? (ADCValue * VPS) *-1: ADCValue * VPS ) ;

            // we write value to the inactive robin
            // store value of ADC0
            adcRoundRobin[adcRoundRobinIndex ? 0 : 1].value[0] = ADCValue;
            // todo: values of ADC 1 - 3

            // after value(s) written, we activate the inactive robin
            adcRoundRobinIndex = adcRoundRobinIndex ? 0 : 1;


            System_printf("ADCValue= %d, ADCVolts= %f \n", ADCValue, ADCVoltage );
            System_flush();
        }
        else {
            System_printf("ADC Read I2C Bus fault\n");
            System_flush();
        }


    }

}

uint16_t getAdc(uint32_t uModule) {
    return adcRoundRobin[adcRoundRobinIndex].value[uModule];
}


