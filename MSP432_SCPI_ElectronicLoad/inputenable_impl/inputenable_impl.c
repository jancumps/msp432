/*
 * inputenable_impl.c
 *
 *  Created on: 3 dec. 2017
 *      Author: jancu
 */

/* XDCtools Header files */
#include <inputenable_impl/inputenable_impl.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/cfg/global.h> // needed to get the global from the .cfg file

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
//#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include "i2c_impl.h"


// the address is 0011000
#define PORTEXTENDER_I2C_ADDR           (0x18)

// storage for the state
volatile bool bInputEnable_State = false;

uint8_t         d_txBuffer[2];
uint8_t         d_rxBuffer[1]; // DAC doesn't read
I2C_Transaction d_i2cTransaction;

/*
 *  ======== fnTaskInputEnable ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Void fnTaskInputEnable(UArg arg0, UArg arg1)
{
    MsgInputEnable d_msg;
    d_i2cTransaction.writeBuf = d_txBuffer;
    d_i2cTransaction.readBuf = d_rxBuffer;
    d_i2cTransaction.slaveAddress = PORTEXTENDER_I2C_ADDR;
    d_i2cTransaction.writeCount = 2;
    d_i2cTransaction.readCount = 0;

    // initialise and set output to off

    // first set all outputs to high

    d_txBuffer[0] = 0x01; // control register: select output port register
    d_txBuffer[1] = 0xFF; // set each bit high in the output register
    if (! I2C_transfer(i2c_implGetHandle(), &d_i2cTransaction)) {
        System_printf("I2C Bus fault\n");
        System_flush();
    }

    d_txBuffer[0] = 0x03; // control register: select configuration register
    d_txBuffer[1] = 0x00; // set each bit low so that all 8 pins are outputs (recommended state for unused pins)
    if (! I2C_transfer(i2c_implGetHandle(), &d_i2cTransaction)) {
        System_printf("I2C Bus fault\n");
        System_flush();
    }


    // from now on we only write to the output port register.
    // Value d_txBuffer[0] can be fixed outside the loop

    d_txBuffer[0] = 0x01; // control register: select output port register

    while (1) {

        /* wait for mailbox to be posted by writer() */
        if (Mailbox_pend(mbInputEnable, &d_msg, BIOS_WAIT_FOREVER)) {

            d_txBuffer[1] = d_msg.value ? 0x3F : 0xFF; // bit 7 low is output enable.
            if (! I2C_transfer(i2c_implGetHandle(), &d_i2cTransaction)) {
                System_printf("I2C Bus fault\n");
                System_flush();
            } else {
                bInputEnable_State = d_msg.value;
            }

        }
    }
}

bool inputenableImplGetInputEnable() {
    return bInputEnable_State;
}

