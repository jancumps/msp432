/*
 * dac_impl.c
 *
 *  Created on: 2 mrt. 2017
 *      Author: jancu
 */

/* XDCtools Header files */
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

/* Example/Board Header files */
#include "Board.h"

#include "i2c_impl.h"
#include "dac_impl.h"

#define DAC_I2C_ADDR           (0x4C)


uint8_t         txBuffer[3];
uint8_t         rxBuffer[1]; // DAC doesn't read
I2C_Transaction i2cTransaction;
MsgDAC msg;


/*
 *  ======== fnTaskDAC ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Void fnTaskDAC(UArg arg0, UArg arg1)
{
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.slaveAddress = DAC_I2C_ADDR;
    i2cTransaction.writeCount = 3;
    i2cTransaction.readCount = 0;
    txBuffer[0] = 0x10; // set value direct

    while (1) {

        /* wait for mailbox to be posted by writer() */
        if (Mailbox_pend(mbDAC, &msg, BIOS_WAIT_FOREVER)) {

            txBuffer[1] = msg.value >> 8; // MSB
            txBuffer[2] = msg.value; // LSB
            if (! I2C_transfer(i2c_implGetHandle(), &i2cTransaction)) {
                System_printf("I2C Bus fault\n");
                System_flush();
            }
        }

    }

}
