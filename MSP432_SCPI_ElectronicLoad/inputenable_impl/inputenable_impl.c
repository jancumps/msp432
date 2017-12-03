/*
 * outputenable_impl.c
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

uint8_t         d_txBuffer[3];
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
    d_i2cTransaction.writeCount = 3;
    d_i2cTransaction.readCount = 0;

    // initialise and set output to off
    // todo



    while (1) {

        /* wait for mailbox to be posted by writer() */
        if (Mailbox_pend(mbInputEnable, &d_msg, BIOS_WAIT_FOREVER)) {
//            d_txBuffer[0] = // todo
//            d_txBuffer[1] = // todo >> 8; // MSB
//            d_txBuffer[2] = // todo; // LSB
//            if (! I2C_transfer(i2c_implGetHandle(), &d_i2cTransaction)) {
//                System_printf("I2C Bus fault\n");
//                System_flush();
//            } else {
        //}
            // bInputEnable_State = todo ;
            bInputEnable_State = d_msg.value;
        }
    }
}

bool inputenableImplGetInputEnable() {
    return bInputEnable_State;
}

