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

// address 7 - 6: 0, load mode 5 - 4: 01 direct from i2c, 3: reserved 0, 2 - 1: channel select, 0: pwr down 0
#define DAC857X_CFG_H0 0b00010000
#define DAC857X_CFG_H1 0b00010010
#define DAC857X_CFG_H2 0b00010100
#define DAC857X_CFG_H3 0b00010110


uint8_t         d_dactxBuffer[3];
uint8_t         d_dacrxBuffer[1]; // DAC doesn't read
I2C_Transaction d_daci2cTransaction;

static const uint8_t array_DAC857X_CFG_H[4] = {DAC857X_CFG_H0, DAC857X_CFG_H1, DAC857X_CFG_H2, DAC857X_CFG_H3};


uint8_t getAddressFromModule(uint8_t module);

/*
 *  ======== fnTaskDAC ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Void fnTaskDAC(UArg arg0, UArg arg1)
{
    // int iSize = MSGDAC_SIZE; // 4 todo comment out. This is only used to set the right value in the RTOS mailbox config

    MsgDAC d_msg;
    d_daci2cTransaction.writeBuf = d_dactxBuffer;
    d_daci2cTransaction.readBuf = d_dacrxBuffer;
    d_daci2cTransaction.slaveAddress = DAC_I2C_ADDR;
    d_daci2cTransaction.writeCount = 3;
    d_daci2cTransaction.readCount = 0;

    while (1) {

        /* wait for mailbox to be posted by writer() */
        if (Mailbox_pend(mbDAC, &d_msg, BIOS_WAIT_FOREVER)) {
            d_dactxBuffer[0] = getAddressFromModule(d_msg.module); // set value direct
            d_dactxBuffer[1] = d_msg.value >> 8; // MSB
            d_dactxBuffer[2] = d_msg.value; // LSB
            if (! I2C_transfer(i2c_implGetHandle(), &d_daci2cTransaction)) {
                System_printf("I2C Bus fault\n");
                System_flush();
            }
        }
    }
}


/**
 * get the hex address for the requested DAC module
 */
uint8_t getAddressFromModule(uint8_t module) {
    return array_DAC857X_CFG_H[module];
}
