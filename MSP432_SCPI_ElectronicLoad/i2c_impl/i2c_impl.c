/*
 * i2c_impl.c
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
#include <ti/drivers/I2C.h>

/* Example/Board Header files */
#include "Board.h"

#include "i2c_impl.h"


#define I2C_IMPL_I2C_PORT               Board_I2C0

/**
 *
 * Initialise the i2c port at first call, return handle on any future calls
 */
I2C_Handle i2c_implGetHandle() {
    static I2C_Handle i2c = NULL;
    if (! i2c) {
        I2C_Params      i2cParams;
        /* Create I2C for usage */
        I2C_Params_init(&i2cParams);
        i2cParams.bitRate = I2C_400kHz;
        i2c = I2C_open(I2C_IMPL_I2C_PORT, &i2cParams);
        if (i2c == NULL) {
//            System_abort("Error Initializing I2C\n");
        }
        else {
//            System_printf("I2C Initialized!\n");
        }
    }

    return i2c;
}
