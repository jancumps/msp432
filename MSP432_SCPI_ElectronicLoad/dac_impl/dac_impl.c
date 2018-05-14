/*
 * dac_impl.c
 *
 *  Created on: 2 mrt. 2017
 *      Author: jancu
 */

#include <mqueue.h>

#include <ti/drivers/I2C.h>

/* Board Header files */
#include "Board.h"

#include "i2c_impl.h"
#include "dac_impl.h"

#include "rtos_schedules.h"

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
 *  ======== threadDAC ========
 */
void *threadDAC(void *arg0) {
    // int iSize = MSGDAC_SIZE; // 4 todo comment out. This is only used to set the right value in the RTOS mailbox config

    MsgDAC d_msg;
    d_daci2cTransaction.writeBuf = d_dactxBuffer;
    d_daci2cTransaction.readBuf = d_dacrxBuffer;
    d_daci2cTransaction.slaveAddress = DAC_I2C_ADDR;
    d_daci2cTransaction.writeCount = 3;
    d_daci2cTransaction.readCount = 0;

    mqd_t mq;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 1;
    attr.mq_msgsize = MSGDAC_SIZE;
    attr.mq_curmsgs = 0;
    mq = mq_open(QUEUE_NAME_DAC, O_CREAT | O_RDONLY, 0644, &attr);

    while (1) {
        ssize_t bytes_read;
        bytes_read = mq_receive(mq, (char *)&d_msg, MSGDAC_SIZE, NULL);

        /* wait for mailbox to be posted by writer() */
        if (bytes_read) {
            d_dactxBuffer[0] = getAddressFromModule(d_msg.module); // set value direct
            d_dactxBuffer[1] = d_msg.value >> 8; // MSB
            d_dactxBuffer[2] = d_msg.value; // LSB
            if (! I2C_transfer(i2c_implGetHandle(), &d_daci2cTransaction)) {
//                System_printf("I2C Bus fault\n");
//                System_flush();
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
