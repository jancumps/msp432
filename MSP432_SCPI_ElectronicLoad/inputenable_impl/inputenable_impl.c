/*
 * inputenable_impl.c
 *
 *  Created on: 3 dec. 2017
 *      Author: jancu
 */

#include <mqueue.h>

#include <ti/drivers/I2C.h>
#include "i2c_impl.h"
#include <inputenable_impl.h>

#include "rtos_schedules.h"


// the address is 0011000
#define PORTEXTENDER_I2C_ADDR           (0x18)

// storage for the state
volatile bool bInputEnable_State = false;

uint8_t         d_iotxBuffer[2];
uint8_t         d_iorxBuffer[1]; // DAC doesn't read
I2C_Transaction d_ioi2cTransaction;

/*
 *  ======== threadTemperatureOverProtection ========

 */
void *threadInputEnable(void *arg0) {
    // int iSize = MSGINPUTENABLE_SIZE; // 1 todo comment out. This is only used to set the right value in the RTOS mailbox config

    MsgInputEnable d_msg;
    d_ioi2cTransaction.writeBuf = d_iotxBuffer;
    d_ioi2cTransaction.readBuf = d_iorxBuffer;
    d_ioi2cTransaction.slaveAddress = PORTEXTENDER_I2C_ADDR;
    d_ioi2cTransaction.writeCount = 2;
    d_ioi2cTransaction.readCount = 0;

    // initialise and set output to off

    // first set all outputs to high

    d_iotxBuffer[0] = 0x01; // control register: select output port register
    d_iotxBuffer[1] = 0xFF; // set each bit high in the output register
    if (! I2C_transfer(i2c_implGetHandle(), &d_ioi2cTransaction)) {
//        System_printf("I2C Bus fault\n");
//        System_flush();
    }

    d_iotxBuffer[0] = 0x03; // control register: select configuration register
    d_iotxBuffer[1] = 0x00; // set each bit low so that all 8 pins are outputs (recommended state for unused pins)
    if (! I2C_transfer(i2c_implGetHandle(), &d_ioi2cTransaction)) {
//        System_printf("I2C Bus fault\n");
//        System_flush();
    }


    // from now on we only write to the output port register.
    // Value d_txBuffer[0] can be fixed outside the loop

    d_iotxBuffer[0] = 0x01; // control register: select output port register

    mqd_t mq;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 1;
    attr.mq_msgsize = MSGINPUTENABLE_SIZE;
    attr.mq_curmsgs = 0;
    mq = mq_open(QUEUE_NAME_INPUTENABLE, O_CREAT | O_RDONLY, 0644, &attr);


    while (1) {
        ssize_t bytes_read;
        bytes_read = mq_receive(mq, (char *)&d_msg, MSGINPUTENABLE_SIZE, NULL);

        /* wait for mailbox to be posted by writer() */
        if (bytes_read) {
            d_iotxBuffer[1] = d_msg.value ? 0x3F : 0xFF; // bit 7 low is output enable.
            if (! I2C_transfer(i2c_implGetHandle(), &d_ioi2cTransaction)) {
//                System_printf("I2C Bus fault\n");
//                System_flush();
            } else {
                bInputEnable_State = d_msg.value;
            }

        }
    }
}

bool inputenableImplGetInputEnable() {
    return bInputEnable_State;
}

